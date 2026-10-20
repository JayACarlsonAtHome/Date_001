// DateRunner - build Date_001 and run its Catch2 tests in a window.
//
// Build:  ./bootstrap.sh --gui   (or umk <root>/gui,<uppsrc> DateRunner CLANG -brsH <root>/gui/out/DateRunner)
// Start:  <root>/gui/out/DateRunner [PROJECT_DIR] [--compiler gcc15|clang21|...] [--type release|debug]
//                                    [--clean] [--smoke]
//         --compiler/--type/--clean preset the choices at the top of the window.
//         --smoke presses Run tests itself, prints the final status line and exits:
//         0 when every test passed (a check that the window, build and report all work).
//
// It only drives the tools bootstrap.sh set up: `cmake --build build`, then the test binary
// build/Date with Catch2's XML reporter. Results are read from that XML, so what the window
// shows is exactly what Catch2 reported. Nothing is written outside build/ and /tmp.

#include <CtrlLib/CtrlLib.h>

#include <signal.h>
#include <unistd.h>

using namespace Upp;

// ======================================================================= helpers

static const char *SETSID = "/usr/bin/setsid";

// children run in their own process group, so Stop also ends what cmake started (ninja, cc1plus)
static bool StartChild(LocalProcess& p, const String& exe, const Vector<String>& args, const String& cwd)
{
	if(FileExists(SETSID)) {
		Vector<String> a;
		a << exe;
		a.Append(args);
		return p.Start(SETSID, a, NULL, cwd);
	}
	return p.Start(exe, args, NULL, cwd);
}

static void KillGroup(LocalProcess& p)
{
	int pid = p.GetPid();
	if(pid > 0 && kill(-pid, SIGTERM) != 0)
		kill(pid, SIGTERM);
}

// read everything the child has written so far (LocalProcess reads 1 KB per call)
static bool Drain(LocalProcess& p, String& out)
{
	bool alive = true;
	for(int k = 0; k < 8192; k++) {
		String o;
		alive = p.Read(o);
		out << o;
		if(o.IsEmpty())
			break;
	}
	return alive;
}

// Catch2 test-spec text: a backslash escapes the characters Catch2 treats as syntax
static String SpecEscape(const String& s)
{
	String r;
	for(int c : s) {
		if(c == '\\' || c == ',' || c == '[' || c == ']' || c == '*' || c == '"' || c == '~')
			r.Cat('\\');
		r.Cat(c);
	}
	return r;
}

// the project root: an argument, else two folders above gui/out/DateRunner
static bool IsProjectRoot(const String& dir)
{
	return FileExists(AppendFileName(dir, "CMakeLists.txt")) && FileExists(AppendFileName(dir, "test_main.cpp"))
	       && FileExists(AppendFileName(dir, "source/jc313_Date.h"));
}

static String FindProjectRoot()
{
	const Vector<String>& cl = CommandLine();
	for(int i = 0; i < cl.GetCount(); i++) {
		if(cl[i] == "--compiler" || cl[i] == "--type")
			i++;   // skip the option's value
		else if(!cl[i].StartsWith("--") && IsProjectRoot(NormalizePath(cl[i])))
			return NormalizePath(cl[i]);
	}
	String d = GetFileFolder(GetExeFilePath());
	for(int up = 0; up < 4 && d.GetCount() > 1; up++) {
		if(IsProjectRoot(d))
			return d;
		d = GetFileFolder(d);
	}
	return Null;
}

// ======================================================================= compilers

struct Toolchain {
	String cxx;      // full path
	String folder;   // build folder prefix: gcc15, clang21, ... (same names as bootstrap.sh)
	String label;    // for the drop list
	String name;     // Clang, GCC: the drop list is sorted by name, then version
	int    major = 0;
};

// first line of `cxx --version`, and the major version from -dumpversion
static String FirstLine(const String& s)  { int n = s.Find('\n'); return TrimBoth(n >= 0 ? s.Left(n) : s); }

// the compilers installed here, one entry per real binary, sorted by name then version
//   (Clang 21, GCC 11, GCC 14, GCC 15); *preferred is the newest GCC, the default choice
static Array<Toolchain> FindToolchains(String *preferred)
{
	const char *cand[] = { "/opt/rh/gcc-toolset-15/root/usr/bin/g++", "g++-15",
	                       "/opt/rh/gcc-toolset-14/root/usr/bin/g++", "g++-14",
	                       "clang++", "g++-13", "g++" };
	Array<Toolchain> list;
	Index<String> seen;
	for(const char *c : cand) {
		String path = *c == '/' ? String(c) : GetFileOnPath(c, GetEnv("PATH"));
		if(path.IsEmpty() || !FileExists(path))
			continue;
		String real = GetFullPath(path);
		char buf[4096];
		ssize_t n = readlink(path, buf, sizeof(buf) - 1);
		if(n > 0) { buf[n] = 0; real = NormalizePath(buf, GetFileFolder(path)); }
		if(seen.Find(real) >= 0)
			continue;
		seen.Add(real);
		String version = FirstLine(Sys(path + " --version"));
		String major = FirstLine(Sys(path + " -dumpversion"));
		int dot = major.Find('.');
		if(dot >= 0)
			major = major.Left(dot);
		if(version.IsEmpty() || major.IsEmpty())
			continue;
		bool clang = version.Find("clang") >= 0;
		Toolchain& t = list.Add();
		t.cxx = path;
		t.folder = (clang ? "clang" : "gcc") + major;
		t.name = clang ? "Clang" : "GCC";
		t.major = ScanInt(major);
		t.label = t.name + " " + major + "   (" + path + ")";
		if(preferred && preferred->IsEmpty())
			*preferred = t.folder;   // the candidates are tried newest GCC first
	}
	Sort(list, [](const Toolchain& a, const Toolchain& b) {
		return a.name != b.name ? a.name < b.name : a.major < b.major;
	});
	return list;
}

// ======================================================================= results

struct Check {          // one failed check (or exception / fatal error / FAIL) inside a test case
	String kind, file, original, expanded;
	int    line = 0;
};

struct CaseResult {
	String name, tags, file;
	int    line = 0;
	bool   done = false, success = false;
	double seconds = 0;
	Array<Check>  failures;
	String stdOut, stdErr;
};

static void CollectFailures(const XmlNode& n, CaseResult& c)
{
	for(int i = 0; i < n.GetCount(); i++) {
		const XmlNode& k = n[i];
		if(!k.IsTag())
			continue;
		String tag = k.GetTag();
		if(tag == "Expression" && k.Attr("success") == "false") {
			Check& f = c.failures.Add();
			f.kind = k.Attr("type");
			f.file = k.Attr("filename");
			f.line = k.AttrInt("line", 0);
			f.original = TrimBoth(k["Original"].GatherText());
			f.expanded = TrimBoth(k["Expanded"].GatherText());
		}
		else if(tag == "Exception" || tag == "FatalErrorCondition" || tag == "Failure") {
			Check& f = c.failures.Add();
			f.kind = tag;
			f.file = k.Attr("filename");
			f.line = k.AttrInt("line", 0);
			f.original = TrimBoth(k.GatherText());
		}
		else if(tag == "Section")
			CollectFailures(k, c);
	}
}

// the whole Catch2 XML report -> cases; false when it is not a Catch2 report
static bool ParseReport(const String& xml, Array<CaseResult>& cases, int& assertsOk, int& assertsFailed)
{
	XmlNode doc = ParseXML(xml);
	const XmlNode& run = doc["Catch2TestRun"];
	if(!run.IsTag())
		return false;
	cases.Clear();
	for(int i = 0; i < run.GetCount(); i++) {
		const XmlNode& t = run[i];
		if(t.IsTag("TestCase")) {
			CaseResult& c = cases.Add();
			c.name = t.Attr("name");
			c.tags = t.Attr("tags");
			c.file = t.Attr("filename");
			c.line = t.AttrInt("line", 0);
			const XmlNode& r = t["OverallResult"];
			c.done = r.IsTag();
			c.success = r.Attr("success") == "true";
			c.seconds = ScanDouble(r.Attr("durationInSeconds"));
			c.stdOut = TrimBoth(r["StdOut"].GatherText());
			c.stdErr = TrimBoth(r["StdErr"].GatherText());
			CollectFailures(t, c);
		}
		else if(t.IsTag("OverallResults")) {
			assertsOk = t.AttrInt("successes", 0);
			assertsFailed = t.AttrInt("failures", 0);
		}
	}
	return true;
}

// ======================================================================= window

struct DateRunner : TopWindow {
	typedef DateRunner CLASSNAME;

	String root, buildDir, testExe, xmlPath;

	Button    bBuild, bRun, bRunSel, bStop;
	Label     lCompiler, lType, lTag, lName;
	DropList  compiler, buildType, tag;
	Option    cleanBuild;
	Array<Toolchain> toolchains;
	EditString name;
	Option    skipSlow;
	ProgressIndicator prog;
	Label     status;
	Splitter  split;
	ArrayCtrl results;
	TabCtrl   tabs;
	LineEdit  detail, log;

	// one step at a time: each is a command line, run in root, then OnStepDone(kind)
	enum Kind { CONFIGURE, BUILD, LIST_TAGS, LIST_TESTS, RUN };
	struct Step { Kind kind; String exe; Vector<String> args; };
	Array<Step> queue;
	Kind        current = BUILD;
	bool        busy = false;
	bool        stopped = false;   // Stop was pressed: a failing step says "stopped", not "failed"
	int         pendingRun = 0;    // Run pressed while a step was running: 1 = all, 2 = selected
	LocalProcess proc;
	String      out;          // the running step's output, not yet shown
	String      stepText;     // all of the running step's output (the build summary reads it)
	String      buildSummary; // "Build OK: ..." of the last build, kept for the status line
	TimeCallback tick;
	dword       runStart = 0;

	bool        smoke = false;     // --smoke: run once, print the status, exit
	int         smokeRc = 1;

	Array<CaseResult> cases;
	Vector<String>    filters;   // the Catch2 test spec of the current run

	DateRunner();
	~DateRunner() { if(busy) KillGroup(proc); }

	void Log(const String& s)          { log.Append(s); log.SetCursor(log.GetLength()); }
	void SetBusy(bool b);
	void Enqueue(Kind k, const String& exe, const Vector<String>& args);
	void EnqueueBuild();
	void StartNext();
	void Poll();
	void OnStepDone(Kind k, int rc);
	void FinishRun();
	void ShowRows();
	void ShowDetail();
	void StartRun(bool selectedOnly);
	void Stop();
	void LoadTags();
	void SelectBuild();
	String BuildName() const;
	Vector<String> CurrentFilters() const;
};

DateRunner::DateRunner()
{
	root = FindProjectRoot();
	xmlPath = GetTempFileName("DateRunner") + ".xml";

	Title("DateRunner - " + root).Sizeable().Zoomable().SetRect(0, 0, Zx(1060), Zy(720));

	int y = 8;
	Add(lCompiler.SetText("Compiler").LeftPosZ(8, 60).TopPosZ(y + 2, 22));
	Add(compiler.LeftPosZ(70, 420).TopPosZ(y + 2, 22));
	Add(lType.SetText("Build type").LeftPosZ(502, 66).TopPosZ(y + 2, 22));
	Add(buildType.LeftPosZ(570, 90).TopPosZ(y + 2, 22));
	Add(cleanBuild.SetLabel("Clean rebuild (compile everything)").LeftPosZ(674, 230).TopPosZ(y + 2, 22));
	Add(bBuild.SetLabel("Build").LeftPosZ(910, 80).TopPosZ(y, 26));
	y += 34;
	Add(bRun.SetLabel("Run tests").LeftPosZ(8, 176).TopPosZ(y, 26));
	Add(bRunSel.SetLabel("Run selected").LeftPosZ(190, 100).TopPosZ(y, 26));
	Add(bStop.SetLabel("Stop").LeftPosZ(296, 70).TopPosZ(y, 26));
	Add(lTag.SetText("Tag").LeftPosZ(386, 28).TopPosZ(y + 2, 22));
	Add(tag.LeftPosZ(416, 160).TopPosZ(y + 2, 22));
	Add(lName.SetText("Name contains").LeftPosZ(588, 92).TopPosZ(y + 2, 22));
	Add(name.LeftPosZ(682, 170).TopPosZ(y + 2, 22));
	Add(skipSlow.SetLabel("Skip [slow] (the two whole-range walks)").HSizePosZ(866, 8).TopPosZ(y + 2, 22));
	y += 34;
	Add(prog.HSizePosZ(8, 8).TopPosZ(y, 16));
	y += 20;
	Add(status.HSizePosZ(8, 8).TopPosZ(y, 22));
	y += 26;
	Add(split.HSizePosZ(8, 8).VSizePosZ(y, 8));

	results.AddIndex("IDX");
	results.AddColumn("#", 40).Sorting();
	results.AddColumn("Test case", 520).Sorting();
	results.AddColumn("Tags", 170).Sorting();
	results.AddColumn("Result", 80);
	results.AddColumn("Failed checks", 90).Sorting();
	results.AddColumn("Time (ms)", 80).Sorting();
	results.EvenRowColor();
	results.WhenSel = THISBACK(ShowDetail);

	detail.SetReadOnly();
	log.SetReadOnly();
	detail.SetFont(Monospace(Zy(11)));
	log.SetFont(Monospace(Zy(11)));
	tabs.Add(detail.SizePos(), "Details");
	tabs.Add(log.SizePos(), "Log");
	split.Vert(results, tabs).SetPos(6200);

	tag.Add("", "All tags");
	tag.SetIndex(0);
	skipSlow = false;
	cleanBuild = false;

	String preferred;
	toolchains = FindToolchains(&preferred);
	for(int i = 0; i < toolchains.GetCount(); i++) {
		compiler.Add(i, toolchains[i].label);
		if(toolchains[i].folder == preferred)
			compiler.SetIndex(i);
	}
	if(toolchains.GetCount() && compiler.GetIndex() < 0)
		compiler.SetIndex(0);
	buildType.Add("release", "Release");
	buildType.Add("debug", "Debug");
	buildType.SetIndex(0);
	compiler << THISBACK(SelectBuild);
	buildType << THISBACK(SelectBuild);

	bBuild << [this] {
		if(busy)
			return;
		stopped = false;
		queue.Clear();
		log.Clear();
		EnqueueBuild();
		Enqueue(LIST_TAGS, testExe, {"--list-tags"});
		StartNext();
	};
	bRun << [this] { StartRun(false); };
	bRunSel << [this] { StartRun(true); };
	bStop << THISBACK(Stop);
	SetBusy(false);

	if(IsNull(root) || toolchains.IsEmpty()) {
		status.SetText(IsNull(root) ? "Project not found: start it as gui/out/DateRunner inside Date_001, or pass the folder."
		                            : "No C++ compiler found (g++ or clang++). Run ./bootstrap.sh first.");
		root = Null;
		SetBusy(false);
		return;
	}
	SelectBuild();
}

// build/<compiler><major>-<type>, e.g. build/gcc15-release: one folder per choice, so switching
// compiler or type never mixes object files and each stays built
String DateRunner::BuildName() const
{
	int i = compiler.GetIndex();
	return (i >= 0 ? toolchains[i].folder : String("none")) + "-" + AsString(~buildType);
}

void DateRunner::SelectBuild()
{
	buildDir = AppendFileName(AppendFileName(root, "build"), BuildName());
	testExe = AppendFileName(buildDir, "Date");
	cases.Clear();
	ShowRows();
	if(FileExists(testExe)) {
		status.SetText("build/" + BuildName() + " is built. Run tests rebuilds what changed first, then runs.");
		if(!busy)
			LoadTags();
	}
	else
		status.SetText("build/" + BuildName() + " is not built yet. Build (or Run tests) will configure and build it.");
}

void DateRunner::SetBusy(bool b)
{
	busy = b;
	bBuild.Enable(!b && !IsNull(root));
	bRun.Enable(!b && !IsNull(root));
	bRunSel.Enable(!b && !IsNull(root) && results.IsCursor());
	bStop.Enable(b);
	tag.Enable(!b);
	name.Enable(!b);
	skipSlow.Enable(!b);
	compiler.Enable(!b);
	buildType.Enable(!b);
	cleanBuild.Enable(!b);
}

void DateRunner::Enqueue(Kind k, const String& exe, const Vector<String>& args)
{
	Step& s = queue.Add();
	s.kind = k;
	s.exe = exe;
	s.args = clone(args);
}

void DateRunner::EnqueueBuild()
{
	const Toolchain& t = toolchains[max(compiler.GetIndex(), 0)];
	if(!FileExists(AppendFileName(buildDir, "CMakeCache.txt"))) {
		Vector<String> a = { "-S", root, "-B", buildDir,
		                     String("-DCMAKE_BUILD_TYPE=") + (AsString(~buildType) == "debug" ? "Debug" : "Release"),
		                     "-DCMAKE_CXX_COMPILER=" + t.cxx };
		if(GetFileOnPath("ninja", GetEnv("PATH")).GetCount())
			a << "-G" << "Ninja";
		Enqueue(CONFIGURE, "cmake", a);
	}
	Vector<String> b = { "--build", buildDir, "-j", AsString(max(CPU_Cores(), 1)) };
	if(cleanBuild)
		b << "--clean-first";
	Enqueue(BUILD, "cmake", b);
}

void DateRunner::StartNext()
{
	if(queue.IsEmpty()) {
		SetBusy(false);
		if(pendingRun) {
			bool sel = pendingRun == 2;
			pendingRun = 0;
			StartRun(sel);
		}
		return;
	}
	Step s = pick(queue[0]);
	queue.Remove(0);
	String exe = s.exe;
	if(exe.Find('/') < 0) {
		String found = GetFileOnPath(exe, GetEnv("PATH"));
		if(found.GetCount())
			exe = found;
	}
	current = s.kind;
	out.Clear();
	stepText.Clear();
	String line = exe;
	for(const String& a : s.args)
		line << ' ' << a;
	if(s.kind != LIST_TAGS && s.kind != LIST_TESTS)
		Log("\n$ " + line + "\n");
	if(!StartChild(proc, exe, s.args, root)) {
		Log("could not start: " + exe + "\n");
		status.SetText("Could not start " + exe + " (see Log).");
		queue.Clear();
		SetBusy(false);
		return;
	}
	SetBusy(true);
	if(s.kind == CONFIGURE || s.kind == BUILD)
		tabs.Set(1);   // show the Log while building
	if(s.kind == CONFIGURE)  status.SetText("Configuring build/" + BuildName() + " ...");
	if(s.kind == BUILD)      status.SetText(String(cleanBuild ? "Clean rebuild of" : "Building") + " build/" + BuildName() + " ...");
	if(s.kind == RUN)        status.SetText("Running tests ...");
	tick.Set(-100, THISBACK(Poll));
}

void DateRunner::Poll()
{
	bool alive = Drain(proc, out);
	if(current == CONFIGURE || current == BUILD)
		stepText << out;
	if(current != LIST_TAGS && current != LIST_TESTS && out.GetCount()) {
		Log(out);
		out.Clear();
	}
	if(current == RUN) {
		// progress: Catch2 writes each test case's OverallResult as the case ends
		String xml = LoadFile(xmlPath);
		int n = 0;
		for(int p = xml.Find("<OverallResult "); p >= 0; p = xml.Find("<OverallResult ", p + 1))
			n++;
		prog.Set(min(n, cases.GetCount()), max(cases.GetCount(), 1));
		status.SetText(Format("Running tests ... %d / %d  (%.1f s)", n, cases.GetCount(), (msecs() - runStart) / 1000.0));
	}
	if(alive && proc.IsRunning())
		return;
	tick.Kill();
	Drain(proc, out);
	if(current == CONFIGURE || current == BUILD)
		stepText << out;
	if(current != LIST_TAGS && current != LIST_TESTS && out.GetCount()) {
		Log(out);
		out.Clear();
	}
	OnStepDone(current, proc.GetExitCode());
}

void DateRunner::OnStepDone(Kind k, int rc)
{
	if(smoke && rc != 0 && (k == CONFIGURE || k == BUILD)) {
		printf("smoke: %s failed\n%s\n", k == CONFIGURE ? "configure" : "build", ~log.Get());
		fflush(stdout);
		Break();
		return;
	}
	if(stopped) {
		status.SetText("Stopped.");
		queue.Clear();
		SetBusy(false);
		return;
	}
	if(k == CONFIGURE || k == BUILD) {
		if(rc != 0) {
			status.SetText(k == CONFIGURE ? "Configure failed (see Log)." : "Build failed (see Log).");
			tabs.Set(1);
			queue.Clear();
			SetBusy(false);
			return;
		}
		if(k == BUILD) {
			// ninja prints [done/total] per step; make prints [ NN%]; both say when nothing was to do
			int steps = 0;
			for(const String& l : Split(stepText, '\n')) {
				int a = l.Find('['), b = l.Find('/');
				if(a == 0 && b > a && l.Find(']') > b)
					steps = max(steps, ScanInt(l.Mid(b + 1)));
			}
			bool none = stepText.Find("no work to do") >= 0 || (steps == 0 && stepText.Find("Building") < 0);
			buildSummary = "Build OK: build/" + BuildName() + (none ? " was already up to date"
			               : Format(" (%d steps%s)", steps, cleanBuild ? ", clean rebuild" : ""));
			status.SetText(buildSummary + ".");
		}
	}
	else if(k == LIST_TAGS) {
		String keep = ~tag;
		tag.Clear();
		tag.Add("", "All tags");
		for(String l : Split(out, '\n')) {
			int b = l.Find('[');
			if(b >= 0)
				tag.Add(TrimBoth(l.Mid(b)));
		}
		tag <<= keep;
		if(tag.GetIndex() < 0)
			tag.SetIndex(0);
		if(buildSummary.GetCount() && queue.IsEmpty())
			status.SetText(buildSummary + ".   " + AsString(tag.GetCount() - 1) + " tags.");
	}
	else if(k == LIST_TESTS) {
		cases.Clear();
		for(String l : Split(out, '\n')) {
			l = TrimRight(l);
			if(l.GetCount())
				cases.Add().name = l;
		}
		ShowRows();
		if(cases.IsEmpty()) {
			status.SetText("No test case matches the filters.");
			queue.Clear();
			SetBusy(false);
			return;
		}
		prog.Set(0, cases.GetCount());
		FileDelete(xmlPath);
		runStart = msecs();
	}
	else if(k == RUN) {
		FinishRun();
		return;
	}
	StartNext();
}

Vector<String> DateRunner::CurrentFilters() const
{
	Vector<String> f;
	String t = ~tag;
	if(t.GetCount())
		f << t;
	String n = TrimBoth(~name);
	if(n.GetCount())
		f << "*" + SpecEscape(n) + "*";
	if(skipSlow)
		f << "~[slow]";
	return f;
}

void DateRunner::StartRun(bool selectedOnly)
{
	if(busy) {
		pendingRun = selectedOnly ? 2 : 1;   // runs when the current step ends
		return;
	}
	if(selectedOnly) {
		if(!results.IsCursor())
			return;
		int i = results.Get(results.GetCursor(), "IDX");
		if(i < 0 || i >= cases.GetCount())
			return;
		filters.Clear();
		filters << SpecEscape(cases[i].name);   // an exact test case name
	}
	else
		filters = CurrentFilters();

	stopped = false;
	queue.Clear();
	log.Clear();
	EnqueueBuild();
	Vector<String> l = { "--list-tests", "--verbosity", "quiet", "--order", "decl" };
	l.Append(clone(filters));
	Enqueue(LIST_TESTS, testExe, l);
	Vector<String> a = { "--reporter", "xml", "--out", xmlPath, "--durations", "yes", "--order", "decl" };
	a.Append(clone(filters));
	Enqueue(RUN, testExe, a);
	StartNext();
}

void DateRunner::FinishRun()
{
	double secs = (msecs() - runStart) / 1000.0;
	int ok = 0, bad = 0;
	String xml = LoadFile(xmlPath);
	if(!ParseReport(xml, cases, ok, bad)) {
		status.SetText("The test run ended without a Catch2 report (crashed or stopped?). See Log.");
		tabs.Set(1);
		ShowRows();
		SetBusy(false);
		return;
	}
	ShowRows();
	int passed = 0, failed = 0;
	for(const CaseResult& c : cases)
		(c.success ? passed : failed)++;
	prog.Set(cases.GetCount(), max(cases.GetCount(), 1));
	status.SetText(Format("%s   %d test cases: %d passed, %d failed.   %d checks passed, %d failed.   %.2f s   (%s)",
	                      failed ? "FAILED" : "ALL PASSED", cases.GetCount(), passed, failed, ok, bad, secs, BuildName()));
	for(int r = 0; r < results.GetCount(); r++) {   // jump to the first failure
		int i = results.Get(r, "IDX");
		if(!cases[i].success) {
			results.SetCursor(r);
			tabs.Set(0);
			break;
		}
	}
	SetBusy(false);
	if(smoke) {
		smokeRc = failed == 0 && cases.GetCount() ? 0 : 1;
		printf("%s\n", ~status.GetText());
		if(smokeRc)
			printf("%s\n", ~detail.Get());   // the first failed case, as the Details tab shows it
		fflush(stdout);
		Break();
	}
}

void DateRunner::ShowRows()
{
	results.Clear();
	for(int i = 0; i < cases.GetCount(); i++) {
		const CaseResult& c = cases[i];
		// a case listed but not yet reported (a run in progress, or one that crashed) shows blanks
		Value res = !c.done ? Value()
		          : c.success ? Value(AttrText("passed").Ink(Green()))
		          : Value(AttrText("FAILED").Ink(Red()).Bold());
		results.Add(i, i + 1, c.name, c.tags, res,
		            c.done ? Value(c.failures.GetCount()) : Value(),
		            c.done ? Value((int)(c.seconds * 1000 + 0.5)) : Value());
	}
	ShowDetail();
}

void DateRunner::ShowDetail()
{
	bRunSel.Enable(!busy && results.IsCursor());
	if(!results.IsCursor()) {
		detail.Clear();
		return;
	}
	int i = results.Get(results.GetCursor(), "IDX");
	if(i < 0 || i >= cases.GetCount())
		return;
	const CaseResult& c = cases[i];
	String s;
	s << c.name << "\n";
	if(c.tags.GetCount())
		s << "tags " << c.tags << "\n";
	if(c.file.GetCount())
		s << c.file << ":" << c.line << "\n";
	if(c.done)
		s << (c.success ? "passed" : "FAILED") << Format(" in %.3f s\n", c.seconds);
	for(const Check& f : c.failures) {
		s << "\n" << f.kind << " at " << GetFileName(f.file) << ":" << f.line << "\n";
		if(f.original.GetCount())
			s << "    " << f.original << "\n";
		if(f.expanded.GetCount() && f.expanded != f.original)
			s << "  with expansion:\n    " << f.expanded << "\n";
	}
	if(c.stdOut.GetCount())
		s << "\nstdout:\n" << c.stdOut << "\n";
	if(c.stdErr.GetCount())
		s << "\nstderr:\n" << c.stdErr << "\n";
	detail.Set(s);
}

void DateRunner::Stop()
{
	if(!busy)
		return;
	stopped = true;
	pendingRun = 0;
	queue.Clear();
	KillGroup(proc);
	Log("\n(stopped)\n");
}

void DateRunner::LoadTags()
{
	buildSummary.Clear();
	queue.Clear();
	Enqueue(LIST_TAGS, testExe, {"--list-tags"});
	StartNext();
}

GUI_APP_MAIN
{
	DateRunner w;
	const Vector<String>& cl = CommandLine();
	for(int i = 0; i < cl.GetCount(); i++) {
		if(cl[i] == "--smoke")
			w.smoke = true;
		else if(cl[i] == "--clean")
			w.cleanBuild = true;
		else if(cl[i] == "--type" && i + 1 < cl.GetCount())
			w.buildType <<= cl[++i];
		else if(cl[i] == "--compiler" && i + 1 < cl.GetCount()) {
			String want = cl[++i];
			for(int k = 0; k < w.toolchains.GetCount(); k++)
				if(w.toolchains[k].folder == want)
					w.compiler.SetIndex(k);
		}
	}
	if(w.buildType.GetIndex() < 0)
		w.buildType.SetIndex(0);
	if(!IsNull(w.root))
		w.SelectBuild();
	if(w.smoke)
		PostCallback([&w] { w.StartRun(false); });
	w.Run();
	if(w.smoke)
		SetExitCode(w.smokeRc);
}
