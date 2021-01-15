#include <cache/test_helpers.hpp>

#include <boost/filesystem.hpp>

#include <formats/yaml/serialize.hpp>
#include <formats/yaml/value_builder.hpp>
#include <fs/blocking/write.hpp>

#include <cache/dump/common.hpp>
#include <cache/dump/operations_file.hpp>

namespace cache {

CacheConfigStatic ConfigFromYaml(const std::string& yaml_string,
                                 const std::string& dump_root,
                                 std::string_view cache_name) {
  formats::yaml::ValueBuilder config_vars(formats::yaml::Type::kObject);
  config_vars["userver-cache-dump-path"] = dump_root;
  components::ComponentConfig source{yaml_config::YamlConfig{
      formats::yaml::FromString(yaml_string), config_vars.ExtractValue()}};
  source.SetName(std::string{cache_name});
  return cache::CacheConfigStatic{source};
}

void CreateDumps(const std::vector<std::string>& filenames,
                 const std::string& directory, std::string_view cache_name) {
  const auto full_directory =
      boost::filesystem::path{directory} / std::string{cache_name};

  fs::blocking::CreateDirectories(full_directory.string());

  for (const auto& filename : filenames) {
    dump::FileWriter writer((full_directory / filename).string(),
                            boost::filesystem::perms::owner_read);
    writer.Write(filename);
    writer.Finish();
  }
}

std::set<std::string> FilenamesInDirectory(const std::string& directory,
                                           std::string_view cache_name) {
  const auto full_directory =
      boost::filesystem::path{directory} / std::string{cache_name};
  if (!boost::filesystem::exists(full_directory)) return {};

  std::set<std::string> result;
  for (const auto& file :
       boost::filesystem::directory_iterator{full_directory}) {
    result.insert(file.path().filename().string());
  }
  return result;
}

}  // namespace cache