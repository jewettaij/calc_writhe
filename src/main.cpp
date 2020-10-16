/// @brief: This program is a small utility which reads a file containing the
///         coordinates of a curve in 3 dimensions (from the standard input),
///         and prints the writhe.  The file should be in 3-column ASCII format
///         (whitespace delimited).
///         Typical usage:
///            calc_writhe < curve_coordinaes.raw


#include <vector>
#include <cassert>
using namespace std;
#include "io.hpp"
#include "err.hpp"
#include "calc_writhe.hpp"

using namespace calc_writhe;


// global variables
const char g_program_name[]   = "calc_writhe";
const char g_version_string[] = "0.2.0";
const char g_date_string[]    = "<2020-6-03>";
const char *g_spaces = " \t"; // whitespace characters (exluding newlines)
const char *g_spaces_and_newlines = " \t\n";





int
main(int argc, char **argv)
{
  try {
    cout.precision(14);

    cerr
      << g_program_name   << ", v"
      << g_version_string << " "
      << g_date_string    << "\n";


    bool is_closed = true;
    double report_progress = 0.0;

    {
      bool syntax_error_occured = false;

      long i=1;
      while (i < argc)
      {
        int ndelete = 0;
        if (strcmp(argv[i], "-open")==0)
        {
          throw InputErr("Error: This program does not yet support automatic closure of open curves.");
          is_closed = false;
          ndelete = 1;
        }
        else if (strcmp(argv[i], "-progress")==0)
        {
          if ((i+1 >= argc) || (argv[i+1][0] == '-')) {
            stringstream err_msg;
            err_msg << "Error: Argument \""<<argv[i]<<"\" should be followed by a number\n"
              "       (the time, in seconds, between progress updates).\n";
            throw InputErr(err_msg.str().c_str());
          }
          report_progress = stof(argv[i+1]);
          ndelete = 2;
        }
        if (ndelete > 0) { // if the argument was recogized
          // Delete the argument(s) recognized in this pass
          for (long j = i; j+ndelete < argc; j++)
            argv[j] = argv[j+ndelete];
          argc -= ndelete;
        }
        else
          i++; //we will deal with unrecongized arguments later

      } // while (i < argc)

    } // command line argument parsing ends here


    // Any remaining arguments?  Hope not.
    if (argc > 1) {
      stringstream err_msg;
      err_msg << "Unnexpected argument: \"" << argv[1] << "\"\n";
      throw InputErr(err_msg.str().c_str());
    }


    // now read in the coordinates from the file

    long n_data_sets = 1;
    vector<double> vX_d;
    vector<vector<double> > vvX_id;
    g_filename.assign("standard-input/terminal");
    g_line=1;           //keep track of which line number
    Skip(cin, g_spaces_and_newlines);
    long prev_line = g_line; //used to figure out if 2 numbers on same line
    long L_min = -1;
    while(cin)
    {
      double x;
      //cin >> x;
      x = ReadScalar<double>(cin, g_spaces_and_newlines);
      if (! cin) break;
      assert(g_line == prev_line);
      vX_d.push_back(x);

      if (cin) Skip(cin, g_spaces_and_newlines);
      //did the line number increment or not?
      if (g_line > prev_line) {
        assert(vX_d.size() > 0);
        if (vX_d.size() != 3)
          throw InputErr("Error: Coordinate file should have 3 numbers on each line.\n");
        vvX_id.push_back(vX_d);
        vX_d.resize(0);
      }
      prev_line = g_line;
    }

    long N = vvX_id.size();

    double writhe =
      CalcWrithe<double, const vector<vector<double> >& >(N,
                                                          vvX_id,
                                                          report_progress);

    cout << writhe << endl;

  } // try
  catch (const std::exception& e) {
    cerr << "\n" << e.what() << endl;
    exit(1);
  }
}
