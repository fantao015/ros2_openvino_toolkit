// Copyright (c) 2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @brief A header file with declaration for HumanPoseEstimation Class
 * @file vehicle_attribs_detection.hpp
 */
#ifndef OPENVINO_WRAPPER_LIB_INFERENCES_VEHICLE_ATTRIBS_DETECTION_HPP_
#define OPENVINO_WRAPPER_LIB_INFERENCES_VEHICLE_ATTRIBS_DETECTION_HPP_
#include <rclcpp/rclcpp.hpp>
#include <memory>
#include <vector>
#include <string>
#include "openvino_wrapper_lib/models/human_pose_estimation_model.hpp"
#include "openvino_wrapper_lib/engines/engine.hpp"
#include "openvino_wrapper_lib/inferences/base_inference.hpp"
#include "openvino_wrapper_lib/inferences/openpose_decoder.hpp"
#include "opencv2/opencv.hpp"
// namespace



namespace openvino_wrapper_lib
{
/**
 * @class HumanPoseEstimationResult
 * @brief Class for storing and processing human pose estimation result.
 */



class HumanPoseEstimationResult : public Result
{
public:
  friend class HumanPoseEstimation;
  explicit HumanPoseEstimationResult(const cv::Rect & location);
  // std::string getColor() const
  // {
  //   return color_;
  // }
  // std::string getType() const
  // {
  //   return type_;
  // }

  HumanPose get_pose() const
  {
    return poses;
  }
private:
  //std::vector<HumanPose> poses;
  HumanPose poses;
  // std::string color_ = "";
  // std::string type_ = "";
};
/**
 * @class HumanPoseEstimation
 * @brief Class to load vehicle attributes detection model and perform detection.
 */
class HumanPoseEstimation : public BaseInference
{
public:
  using Result = openvino_wrapper_lib::HumanPoseEstimationResult;
  HumanPoseEstimation();
  ~HumanPoseEstimation() override;
  /**
   * @brief Load the vehicle attributes detection model.
   */
  void loadNetwork(std::shared_ptr<Models::HumanPoseEstimationModel>);
  /**
   * @brief Enqueue a frame to this class.
   * The frame will be buffered but not infered yet.
   * @param[in] frame The frame to be enqueued.
   * @param[in] input_frame_loc The location of the enqueued frame with respect
   * to the frame generated by the input device.
   * @return Whether this operation is successful.
   */
  bool enqueue(const cv::Mat &, const cv::Rect &) override;
  /**
   * @brief Start inference for all buffered frames.
   * @return Whether this operation is successful.
   */
  bool submitRequest() override;
  /**
   * @brief This function will fetch the results of the previous inference and
   * stores the results in a result buffer array. All buffered frames will be
   * cleared.
   * @return Whether the Inference object fetches a result this time
   */
  bool fetchResults() override;
  /**
   * @brief Get the length of the buffer result array.
   * @return The length of the buffer result array.
   */
  int getResultsLength() const override;
  /**
   * @brief Get the location of result with respect
   * to the frame generated by the input device.
   * @param[in] idx The index of the result.
   */
  const openvino_wrapper_lib::Result * getLocationResult(int idx) const override;
  /**
   * @brief Show the observed detection result either through image window
     or ROS topic.
   */
  void observeOutput(const std::shared_ptr<Outputs::BaseOutput> & output);
  /**
   * @brief Get the name of the Inference instance.
   * @return The name of the Inference instance.
   */
  const std::string getName() const override;
  const std::vector<cv::Rect> getFilteredROIs(
  const std::string filter_conditions) const override;
  void resizeFeatureMaps(std::vector<cv::Mat>& featureMaps);
  std::vector<HumanPose> extractPoses(const std::vector<cv::Mat>& heatMaps,
                                                 const std::vector<cv::Mat>& pafs);

private:
  int upsampleRatio = 4;
  int stride = 8;
  //float minPeaksDistance = 6.0f / (stride / upsampleRatio);
  size_t keypointsNumber = 18;
  int minJointsNumber = 3;
  float minPeaksDistance = 3.0f;
  float midPointsScoreThreshold = 0.05f;
  float foundMidPointsRatioThreshold = 0.8f;
  float minSubsetScore = 0.2f;

  float confidenceThreshold = 0.5;
  cv::Size frame_size;

  std::shared_ptr<Models::HumanPoseEstimationModel> valid_model_;
  std::vector<Result> results_;
};
}  // namespace openvino_wrapper_lib


class FindPeaksBody : public cv::ParallelLoopBody {
public:
    FindPeaksBody(const std::vector<cv::Mat>& heatMaps,
                  float minPeaksDistance,
                  std::vector<std::vector<Peak>>& peaksFromHeatMap,
                  float confidenceThreshold)
        : heatMaps(heatMaps),
          minPeaksDistance(minPeaksDistance),
          peaksFromHeatMap(peaksFromHeatMap),
          confidenceThreshold(confidenceThreshold) {}

    void operator()(const cv::Range& range) const override {
        for (int i = range.start; i < range.end; i++) {
            findPeaks(heatMaps, minPeaksDistance, peaksFromHeatMap, i, confidenceThreshold);
        }
    }

private:
    const std::vector<cv::Mat>& heatMaps;
    float minPeaksDistance;
    std::vector<std::vector<Peak>>& peaksFromHeatMap;
    float confidenceThreshold = 0.5;
};
#endif  // OPENVINO_WRAPPER_LIB_INFERENCES_VEHICLE_ATTRIBS_DETECTION_HPP_