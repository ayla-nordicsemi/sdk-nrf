.. _jwt_application:

jwt generator
###################

.. contents::
   :local:
   :depth: 2

This sample shows how Application core can generate a JWT signed with the IAK.

Requirements
************

The sample supports the following development kits:

.. list-table:: Supported development kits
   :widths: 50 50
   :header-rows: 1

   * - Board
     - Support
   * - nrf54h20dk/nrf54h20/cpuapp
     - Yes

Overview
********

The sample goes through the following steps to generate a JWT:

1. Reads the device UUID.

   The returned UUID is compliant with UUID v4 defined by ITU-T X.667 | ISO/IEC 9834-8.

2. Generates a JWT.

   Uses the user provided fields for audiance and expiration delta, will always use IAK key to sign the JWT.
   The generated JWT is printed to serial terminal if project configuration allows it.

Configuration
*************

|config|

LIB JWT APP
===========

As per provided on the project config, the used APIs requier the usage of lib::app_jwt.

JWT signing verification
========================

Flag :kconfig:option:`CONFIG_APP_JWT_VERIFY_SIGNATURE` allow to verify the JWT signature against the IAK key.

Export public IAK key
=====================

User might be interrested in the DER formatted IAK key for later verifications of the generated JWT, the flag :kconfig:option:`CONFIG_APP_JWT_PRINT_EXPORTED_PUBKEY_DER` allows printing the DER formatted key to debug terminal.

Building and running
********************

   .. code-block:: console

      west build -p -b nrf54h20dk/nrf54h20/cpuapp nrf/samples/jwt --build-dir build_cpuapp_nrf54h20dk_jwt_logging_serial/ -T samples.jwt.logging.uart

Testing
=======

|test_sample|

1. |connect_terminal_specific|
#. Reset the kit.
#. Observe the following output (DER formatted public IAK key, and the JWT):

   .. code-block:: console

    jwt_sample: Appcation JWT sample (nrf54h20dk)
    jwt_sample: App: pubkey_der (91) =  3059301306072a8648ce3d020106082a8648ce3d03010703420004017e627cc237c5a37d9142d0cba1530a5653c4f41e6ba6e06d3b74fdf5c308b09aff1d99946d5deb4dd97dbd0dbcba62c3d9ba518fc9e43be88b780b1484
    jwt_sample: JWT(500) :
    jwt_sample: eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiIsImtpZCI6IjRkMmM2NjdmNTYzMzExY2Q4OWE2N2M2ZWQ4ZDQwZDBmYWIyMzAyNTc3MjIyNjNjNDNkMThiNTM4YzZjN2JjZTkifQ.eyJqdGkiOiJucmY1NGgyMGRrLmZlMDI1ODlhYzA4NzVmY2IzMDk3Y2U5MmFmZGIyMTBlIiwiaXNzIjoibnJmNTRoMjBkay4zNzU4ZTE5NC03MjgyLTExZWYtOTMyYi03YjFmMDcwY2NlN2EiLCJzdWIiOiIzNzU4ZTE5NC03MjgyLTExZWYtOTMyYi03YjFmMDcwY2NlN2EiLCJhdWQiOiJKU09OIHdlYiB0b2tlbiBmb3IgZGVtb25zdHJhdGlvbiIsImV4cCI6NjA0ODAwfQ.w0Bf6QvXBLshvRn1vT9qcLjlFovgx9beOknHl848XeWgpaOWAdzmUZ6eB85-SMnQPZTglGZhcdWSp5mMMQABXg

   If an error occurs, the sample prints an error message.

.. note::
   Due to the extra long strings that the terminal has to print, be sure to configure :kconfig:option:`CONFIG_LOG_BUFFER_SIZE` with enough size to avoid truncated strings on the output.

.. note::
   Currently, the provided Sample doesn't support other boards than nrf54h20dk.

Dependencies
************

This sample uses the following |NCS| libraries:

* :ref:`lib_app_jwt`
