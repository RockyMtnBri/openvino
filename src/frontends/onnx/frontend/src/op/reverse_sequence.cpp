// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/reverse_sequence.hpp"

#include "onnx_import/core/node.hpp"
#include "openvino/frontend/exception.hpp"
#include "openvino/op/convert.hpp"
#include "openvino/op/reverse_sequence.hpp"
#include "validation_util.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
OutputVector reverse_sequence(const Node& node) {
    const auto data = node.get_ng_inputs().at(0);

    const auto sequence_lengths = node.get_ng_inputs().at(1);
    // OpenVINO supports only int32 type of sequence_lengths
    const auto sequence_lengths_i32 = std::make_shared<v0::Convert>(node.get_ng_inputs().at(1), element::i32);
    const auto data_rank = data.get_partial_shape().rank();

    const auto batch_axis = node.get_attribute_value<int64_t>("batch_axis", 1);
    const auto normalized_batch_axis = ov::util::normalize_axis(node.get_description(), batch_axis, data_rank);
    const auto time_axis = node.get_attribute_value<int64_t>("time_axis", 0);
    const auto normalized_time_axis = ov::util::normalize_axis(node.get_description(), time_axis, data_rank);

    FRONT_END_GENERAL_CHECK(normalized_batch_axis == 0 || normalized_batch_axis == 1,
                            "Allowed values of the 'batch_axis' attribute for ReverseSequence "
                            "operator are 0 and 1");

    FRONT_END_GENERAL_CHECK(normalized_time_axis == 0 || normalized_time_axis == 1,
                            "Allowed values of the 'time_axis' attribute for ReverseSequence "
                            "operator are 0 and 1");

    FRONT_END_GENERAL_CHECK(normalized_batch_axis != normalized_time_axis,
                            "'batch_axis' and 'time_axis' attributes of the ReverseSequence "
                            "operator can't point to the same dimension");

    return {
        std::make_shared<v0::ReverseSequence>(data, sequence_lengths_i32, normalized_batch_axis, normalized_time_axis)};
}

}  // namespace set_1

}  // namespace op

}  // namespace onnx_import

}  // namespace ngraph
OPENVINO_SUPPRESS_DEPRECATED_END
