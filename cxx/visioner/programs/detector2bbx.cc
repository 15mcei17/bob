#include <fstream>

#include "visioner/cv/cv_detector.h"
#include "visioner/cv/cv_draw.h"
#include "visioner/util/timer.h"

int main(int argc, char *argv[]) {	

  visioner::CVDetector detector;

  // Parse the command line
  boost::program_options::options_description po_desc("", 160);
  po_desc.add_options()
    ("help,h", "help message");
  po_desc.add_options()
    ("data", boost::program_options::value<std::string>(), 
     "test datasets")
    ("results", boost::program_options::value<std::string>()->default_value("./"),
     "directory to save bounding boxes to");
  detector.add_options(po_desc);

  boost::program_options::variables_map po_vm;
  boost::program_options::store(
      boost::program_options::command_line_parser(argc, argv)
      .options(po_desc).run(),
      po_vm);
  boost::program_options::notify(po_vm);

  // Check arguments and options
  if (	po_vm.empty() || po_vm.count("help") || 
      !po_vm.count("data") ||
      !detector.decode(po_desc, po_vm))
  {
    visioner::log_error("detector2bbx") << po_desc << "\n";
    exit(EXIT_FAILURE);
  }

  const std::string cmd_data = po_vm["data"].as<std::string>();
  const std::string cmd_results = po_vm["results"].as<std::string>();

  // Load the test datasets
  visioner::strings_t ifiles, gfiles;
  if (visioner::load_listfiles(cmd_data, ifiles, gfiles) == false)
  {
    visioner::log_error("detector2bbx") << "Failed to load the test datasets <" << cmd_data << ">!\n";
    exit(EXIT_FAILURE);
  }

  visioner::Timer timer;

  // Process each image ...
  for (std::size_t i = 0; i < ifiles.size(); i ++)
  {
    const std::string& ifile = ifiles[i];
    const std::string& gfile = gfiles[i];

    // Load the image and the ground truth
    if (detector.load(ifile, gfile) == false)
    {
      visioner::log_error("detector2bbx")
        << "Failed to load image <" << ifile << "> or ground truth <" << gfile << ">!\n";
      exit(EXIT_FAILURE);
    }

    timer.restart();

    // Detect objects
    visioner::detections_t detections;                
    visioner::bools_t labels;

    detector.scan(detections);
    detector.label(detections, labels);

    // Save the bounding boxes of the correct detections
    std::ofstream out((cmd_results + "/" + visioner::basename(ifiles[i]) + ".det.bbx").c_str());
    if (out.is_open() == false)
    {
      continue;
    }

    for (std::size_t d = 0; d < detections.size(); d ++)
    {
      if (labels[d] == true)
      {
        const visioner::detection_t& det = detections[d];
        const visioner::rect_t& bbx = det.second.first;
        out << bbx.left() << " " << bbx.top() 
          << " " << bbx.width() << " " << bbx.height() << "\n";
      }
    }

    visioner::log_info("detector2bbx") 
      << "Image [" << (i + 1) << "/" << ifiles.size() << "]: scanned " 
      << detections.size() << "/" << detector.stats().m_sws << " SWs & "
      << detector.n_objects() << "/" << detector.stats().m_gts << " GTs in " 
      << timer.elapsed() << "s.\n";
  }

  // Display statistics
  detector.stats().show();

  // OK
  visioner::log_finished();
  return EXIT_SUCCESS;

}
