// Copyright (c) 2018-2022 Intel Corporation
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

#include "dynamic_vino_lib/outputs/base_output.hpp"
#include "dynamic_vino_lib/pipeline.hpp"

void Outputs::BaseOutput::setPipeline(Pipeline * const pipeline)
{
  pipeline_ = pipeline;
}

Pipeline * Outputs::BaseOutput::getPipeline() const
{
  return pipeline_;
}

cv::Mat Outputs::BaseOutput::getFrame() const
{
  return frame_;
}
