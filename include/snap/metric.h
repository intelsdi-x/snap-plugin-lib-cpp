#include <ctime>
#include <map>
#include <string>

namespace Metric {

// TODO (danielscottt): use a union for data here.
class Metric {

  public:
    void setData(int data);

  private:
    Namespace ns;
    int data;
    time_t lastAdvertisedTime;
    time_t timestamp;
    std::map<std::string, std::string> tags;
    std::string description;
    std::string unit;
}

class Namespace : private std::vector<std::string> {
  public:
    Namespace();
    ~Namespace();
}

} // namespace Metric
