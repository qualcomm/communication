# *******************************************************************************
# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

load("//score/mw:common_features.bzl", "COMPILER_WARNING_FEATURES")

# ============================================================================
# Platform Configuration Settings
# ============================================================================

# Config setting for QNX7
config_setting(
    name = "qnx7",
    constraint_values = [
        "@platforms//os:qnx",
        "@score_bazel_platforms//:qnx7_1",
    ],
)

# Config setting for QNX8
config_setting(
    name = "qnx8",
    constraint_values = [
        "@platforms//os:qnx",
        "@score_bazel_platforms//:qnx8_0",
    ],
)

# Config setting for Linux aarch64 (HGY platform)
config_setting(
    name = "linux_aarch64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:aarch64",
    ],
)

# Config setting for Linux x86_64 (standard GCC)
config_setting(
    name = "linux_x86_64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
)

# Config setting for Android GVM (aarch64)
config_setting(
    name = "android_aarch64",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:aarch64",
    ],
)
# ============================================================================
# Dynamic Library Configuration
# ============================================================================

# Comprehensive aggregation library that includes ALL communication middleware components which Symbols are needed in the SO.
# This ensures all transitive dependencies and their respective Public hdr symbols are properly included in the shared library
# Note: Had to do this for all transitive deps into this com_all_deps package due to Symbol Inclusion issue.
#       Without this addition. Bazel is not adding Transitive Dependency Symbols into so. Refer to SYMBOL_VERSIONING_GUIDE.md for more details on this.
cc_library(
    name = "com_all_deps",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FUSA"],
    visibility = ["//visibility:public"],
    alwayslink = True,  # Force all symbols to be included
    deps = [
        # Public API layer
        "//score/mw/com:com_error_domain",
        "//score/mw/com:runtime",
        "//score/mw/com:runtime_configuration",
        "//score/mw/com:types",
        
        # Implementation layer - core components
        "//score/mw/com/impl:impl",
        "//score/mw/com/impl:runtime",
        "//score/mw/com/impl:error",
        "//score/mw/com/impl:instance_identifier",
        "//score/mw/com/impl:instance_specifier",
        "//score/mw/com/impl:handle_type",
        "//score/mw/com/impl:enriched_instance_identifier",
        
        # Proxy components
        "//score/mw/com/impl:proxy_base",
        "//score/mw/com/impl:proxy_event",
        "//score/mw/com/impl:proxy_event_base",
        "//score/mw/com/impl:proxy_field",
        "//score/mw/com/impl:proxy_field_base",
        "//score/mw/com/impl:generic_proxy",
        "//score/mw/com/impl:generic_proxy_event",
        "//score/mw/com/impl:proxy_binding",
        "//score/mw/com/impl:proxy_event_binding",
        "//score/mw/com/impl:generic_proxy_event_binding",
        
        # Skeleton components
        "//score/mw/com/impl:skeleton_base",
        "//score/mw/com/impl:skeleton_event",
        "//score/mw/com/impl:skeleton_event_base",
        "//score/mw/com/impl:skeleton_field",
        "//score/mw/com/impl:skeleton_field_base",
        "//score/mw/com/impl:skeleton_binding",
        "//score/mw/com/impl:skeleton_event_binding",
        
        # Service discovery
        #"//score/mw/com/impl:find_service_handle",
        #"//score/mw/com/impl:find_service_handler",
        #"//score/mw/com/impl:i_service_discovery",
        #"//score/mw/com/impl:i_service_discovery_client",
        #"//score/mw/com/impl:service_discovery",
        
        # Runtime interfaces
        #"//score/mw/com/impl:runtime_interfaces",
        #"//score/mw/com/impl:event_receive_handler",
        #"//score/mw/com/impl:scoped_event_receive_handler",
        #"//score/mw/com/impl:subscription_state",
        #"//score/mw/com/impl:binding_type",
        #"//score/mw/com/impl:service_element_type",
        #"//score/mw/com/impl:service_element_map",
        #"//score/mw/com/impl:sample_reference_tracker",
        #"//score/mw/com/impl:flag_owner",
        #"//score/mw/com/impl:traits",
        
        # Configuration
        #"//score/mw/com/impl/configuration",
        
        # Plumbing layer
        "//score/mw/com/impl/plumbing:plumbing",
        "//score/mw/com/impl/plumbing:runtime",
        #"//score/mw/com/impl/plumbing:event",
        #"//score/mw/com/impl/plumbing:field",
        #"//score/mw/com/impl/plumbing:sample_ptr",
        #"//score/mw/com/impl/plumbing:sample_allocatee_ptr",
        #"//score/mw/com/impl/plumbing:proxy_binding_factory",
        #"//score/mw/com/impl/plumbing:proxy_event_binding_factory",
        #"//score/mw/com/impl/plumbing:proxy_field_binding_factory",
        #"//score/mw/com/impl/plumbing:skeleton_binding_factory",
        #"//score/mw/com/impl/plumbing:skeleton_event_binding_factory",
        #"//score/mw/com/impl/plumbing:skeleton_field_binding_factory",
        
        # Methods
        #"//score/mw/com/impl/methods:proxy_method",
        #"//score/mw/com/impl/methods:proxy_method_base",
        #"//score/mw/com/impl/methods:proxy_method_binding",
        #"//score/mw/com/impl/methods:method_signature_element_ptr",
        
        # Tracing
        "//score/mw/com/impl/tracing:tracing_runtime",
        #"//score/mw/com/impl/tracing:i_tracing_runtime",
        #"//score/mw/com/impl/tracing:i_tracing_runtime_binding",
        #"//score/mw/com/impl/tracing:skeleton_tracing",
        "//score/mw/com/impl/tracing:skeleton_event_tracing",
        "//score/mw/com/impl/tracing:proxy_event_tracing",
        #"//score/mw/com/impl/tracing:common_event_tracing",
        #"//score/mw/com/impl/tracing:type_erased_sample_ptr",
        #"//score/mw/com/impl/tracing:skeleton_event_tracing_data",
        #"//score/mw/com/impl/tracing:proxy_event_tracing_data",
        #"//score/mw/com/impl/tracing:service_element_tracing_data",
        #"//score/mw/com/impl/tracing:trace_error",
        
        # Bindings - LOLA (main binding implementation)
        "//score/mw/com/impl/bindings/lola:lola",
        "//score/mw/com/impl/bindings/lola:proxy",
        "//score/mw/com/impl/bindings/lola:skeleton",
        "//score/mw/com/impl/bindings/lola:type_erased_sample_ptrs_guard",

        #Score specific Symbols
        "@score_baselibs//score/language/futurecpp:futurecpp",
        "@score_baselibs//score/mw/log:frontend",
    ] + select({
        ":qnx7": ["@score_logging//score/mw/log/backend:slog"],
        ":qnx8": ["@score_logging//score/mw/log/backend:slog"],
        "//conditions:default": [],
    }),
)

# ============================================================================
# Symbol Version Script
# ============================================================================

# Generate version script for symbol versioning on Linux
genrule(
    name = "generate_version_script",
    outs = ["version_script.lds"],
    cmd = """
        cat > $@ << 'EOF'
# Symbol Version Script for libscore_communication.so
# This controls which symbols are exported from the shared library

SCORE_COM_1.0 {
    global:
        # Export all public API symbols
        # C++ symbols will be mangled, so we use wildcards
        # Entire Score
        *score*; 
    local:
        # Hide all other symbols
        *;
};
EOF
    """,
    visibility = ["//visibility:public"],
)

# Provides weak __hash_memory symbol missing from Android's libc++ (aarch64).
cc_library(
    name = "libcxx_android_compat",
    srcs = select({
        "@platforms//os:android": ["libcxx_android_compat.cpp"],
        "//conditions:default": [],
    }),
    target_compatible_with = ["@platforms//os:android"],
    visibility = ["//visibility:public"],
)

# Shared library for deployment containing all COM functionality
# This bundles all communication middleware code into a single .so
# All transitive dependencies are statically linked into this shared library
cc_shared_library(
    name = "score_communication",
    features = COMPILER_WARNING_FEATURES,
    tags = ["FUSA"],
    visibility = ["//visibility:public"],
    deps = [
        ":com_all_deps",  
    ] + select({
        "@platforms//os:android": [":libcxx_android_compat"],
        "//conditions:default": [],
    }),
    # Specify which dependencies should be linked dynamically
    # Empty list means ALL dependencies are statically linked into the shared library
    dynamic_deps = [],
    # Additional linker options for dynamic library
    user_link_flags = select({
        ":linux_aarch64": [
            "-Wl,-soname,libscore_communication.so",
            "-Wl,-rpath,'$$ORIGIN'",  # Set RPATH for runtime library search
            "-Wl,--version-script=$(location :generate_version_script)",  # Symbol versioning
            "-Wl,--allow-multiple-definition",  # Allow duplicate symbols (choose first)
            "-lacl",  # Link against ACL library for file permissions (needed for aarch64)
            "-static-libgcc",  # Statically link libgcc
            "-static-libstdc++",  # Statically link libstdc++
        ],
        ":linux_x86_64": [
            "-Wl,-soname,libscore_communication.so",
            "-Wl,-rpath,'$$ORIGIN'",  # Set RPATH for runtime library search
            "-Wl,--version-script=$(location :generate_version_script)",  # Symbol versioning
            "-Wl,--allow-multiple-definition",  # Allow duplicate symbols (choose first)
            "-static-libgcc",  # Statically link libgcc
            "-static-libstdc++",  # Statically link libstdc++
        ],
        "@platforms//os:macos": [
            "-Wl,-install_name,@rpath/libscore_communication.dylib",
            "-Wl,-rpath,@loader_path",
            "-static-libgcc",
            "-static-libstdc++",
        ],
        ":qnx7": [
            "-Wl,-soname,libscore_communication.so",
            "-Wl,-rpath,'$$ORIGIN'",
            "-Wl,--version-script=$(location :generate_version_script)",  # QNX supports GNU ld version scripts
            "-lslog2",  # QNX logging library (system library, must be dynamic)
            "-Wl,--allow-multiple-definition",  # Allow duplicate symbols (choose first)
            "-static-libgcc",
            "-static-libstdc++",
        ],
        ":qnx8": [
            "-Wl,-soname,libscore_communication.so",
            "-Wl,-rpath,'$$ORIGIN'",
            "-Wl,--version-script=$(location :generate_version_script)",  # QNX supports GNU ld version scripts
            "-lslog2",  # QNX logging library (system library, must be dynamic)
            "-lfsnotify",  # QNX8-specific: fsnotify library (system library, must be dynamic)
            "-Wl,--allow-multiple-definition",  # Allow duplicate symbols (choose first)
            "-static-libgcc",
            "-static-libstdc++",
        ],
        ":android_aarch64": [
            "-Wl,-soname,libscore_communication.so",
            "-Wl,--allow-multiple-definition",
        ],
        "//conditions:default": [],
    }),
    additional_linker_inputs = select({
        ":linux_aarch64": [":generate_version_script"],
        ":linux_x86_64": [":generate_version_script"],
        ":qnx7": [":generate_version_script"],
        ":qnx8": [":generate_version_script"],
        "//conditions:default": [],
    }),
)

