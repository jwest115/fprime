import os
import json
from deepdiff import DeepDiff
from fprime_gds.executables.data_product_writer import DataProductWriter


def test_dp_send(fprime_test_api):
    """Test that DPs are generated and received on the ground"""

    # Run Dp command to send a data product
    fprime_test_api.send_and_assert_command("Ref.dpDemo.Dp", ["IMMEDIATE", 1])
    # Wait for DpStarted event
    result = fprime_test_api.await_event("Ref.dpDemo.DpStarted", start=0, timeout=5)
    assert result
    # Wait for DpComplete event
    result = fprime_test_api.await_event("Ref.dpDemo.DpComplete", start=0, timeout=10)
    assert result
    # Check for FileWritten event and capture the name of the file that was created
    file_result = fprime_test_api.await_event(
        "DataProducts.dpWriter.FileWritten", start=0, timeout=10
    )
    dp_file_path = file_result.get_display_text().split().pop()
    # Verify that the file exists
    # Assumes that we are running the test from the Ref directory
    assert os.path.isfile(dp_file_path)


def test_dp_decode(fprime_test_api):
    """Test that we can decode DPs on the ground via fprime_dp_writer"""

    # Run Dp command to send a data product
    fprime_test_api.send_and_assert_command("Ref.dpDemo.Dp", ["IMMEDIATE", 1])
    # Check for FileWritten event and capture the name of the file that was created
    file_result = fprime_test_api.await_event(
        "DataProducts.dpWriter.FileWritten", start=0, timeout=10
    )
    dp_file_path = file_result.get_display_text().split().pop()
    # Verify that the file exists
    # Assumes that we are running the test from the Ref directory
    assert os.path.isfile(dp_file_path)
    # Decode DP with fprime-dp-writer tool
    json_dict = fprime_test_api.pipeline.dictionary_path
    decoded_file_name = os.path.basename(dp_file_path).replace(".fdp", ".json")
    DataProductWriter(json_dict, dp_file_path).process()
    assert os.path.isfile(decoded_file_name)
    with open("./DpDemo/test/int/dp_ref_output.json", "r") as ref_file, open(
        decoded_file_name, "r"
    ) as output_file:
        ref_json = json.load(ref_file)
        output_json = json.load(output_file)
    assert not DeepDiff(
        ref_json,
        output_json,
        ignore_order=True,
        exclude_paths=[
            "root[0]['Seconds']",
            "root[0]['USeconds']",
            "root[0]['TimeBase']",
            "root[0]['Context']",
            "root[0]['headerHash']",
        ],
    )
