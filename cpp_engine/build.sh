#!/bin/bash

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Change to the cpp_engine directory (where this script lives)
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
COMPILER="g++"
BUILD_DIR="build"

# Help function
show_help() {
    echo "Usage: ./build.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --help              Show this help message"
    echo "  --clean             Remove build directory"
    echo "  --compiler <gcc|clang>  Choose compiler (default: gcc), then clean and build"
    echo "  --build             Build all targets"
    echo "  --test              Build (if needed) and run all tests"
    echo "  --test-all          Build (if needed) and run all tests"
    echo "  --test <name>       Build (if needed) and run specific test"
    echo "  --all               Clean, build, and run all tests"
    echo ""
    echo "Default (no options): Clean and build everything"
    echo ""
    echo "Examples:"
    echo "  ./build.sh                        # Clean and build with gcc"
    echo "  ./build.sh --compiler clang       # Clean and build with clang"
    echo "  ./build.sh --all                  # Clean, build, and test all"
    echo "  ./build.sh --clean --all          # Clean, then build and test all"
    echo "  ./build.sh --test                 # Build and run all tests"
    echo "  ./build.sh --test-all             # Build and run all tests"
    echo "  ./build.sh --test blackscholes    # Build and run specific test"
    echo "  ./build.sh --compiler clang --test # Build with clang and test"
    echo "  ./build.sh --clean                # Just clean"
}

# Clean function
clean() {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        echo -e "${GREEN}✓ Build directory removed${NC}"
    else
        echo -e "${YELLOW}Build directory doesn't exist, nothing to clean${NC}"
    fi
}

# Build function
build() {
    echo -e "${YELLOW}Building with $COMPILER...${NC}"
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    if [ "$COMPILER" == "clang++" ]; then
        cmake -DCMAKE_CXX_COMPILER=clang++ ..
    else
        cmake -DCMAKE_CXX_COMPILER=g++ ..
    fi
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ CMake configuration failed${NC}"
        cd ..
        exit 1
    fi
    
    # Build
    make
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Build failed${NC}"
        cd ..
        exit 1
    fi
    
    echo -e "${GREEN}✓ Build successful${NC}"
    cd ..
}

# Test function
run_tests() {
    TEST_NAME=$1
    
    # Auto-build if build directory doesn't exist
    if [ ! -d "$BUILD_DIR" ]; then
        echo -e "${YELLOW}Build directory doesn't exist. Building first...${NC}"
        build
        echo ""
    fi
    
    cd "$BUILD_DIR"
    
    case "$TEST_NAME" in
        "all"|"")
            echo -e "${YELLOW}Running all tests...${NC}"
            echo ""
            BS_RESULT=0
            PORT_RESULT=0
            RE_RESULT=0
            
            if [ -f "./test_blackscholes" ]; then
                echo -e "${YELLOW}=== BlackScholes Tests ===${NC}"
                ./test_blackscholes
                BS_RESULT=$?
            fi
            echo ""
            
            if [ -f "./test_portfolio" ]; then
                echo -e "${YELLOW}=== Portfolio Tests ===${NC}"
                ./test_portfolio
                PORT_RESULT=$?
            fi
            echo ""
            
            if [ -f "./test_risk_engine" ]; then
                echo -e "${YELLOW}=== RiskEngine Tests ===${NC}"
                ./test_risk_engine
                RE_RESULT=$?
            fi
            echo ""
            
            if [ $BS_RESULT -eq 0 ] && [ $PORT_RESULT -eq 0 ] && [ $RE_RESULT -eq 0 ]; then
                echo -e "${GREEN}✓ All tests passed${NC}"
            else
                echo -e "${RED}✗ Some tests failed${NC}"
                cd ..
                exit 1
            fi
            ;;
        "blackscholes")
            echo -e "${YELLOW}Running BlackScholes tests...${NC}"
            if [ -f "./test_blackscholes" ]; then
                ./test_blackscholes
                if [ $? -eq 0 ]; then
                    echo -e "${GREEN}✓ BlackScholes tests passed${NC}"
                else
                    echo -e "${RED}✗ BlackScholes tests failed${NC}"
                    cd ..
                    exit 1
                fi
            else
                echo -e "${RED}✗ test_blackscholes not found${NC}"
                cd ..
                exit 1
            fi
            ;;
        "portfolio")
            echo -e "${YELLOW}Running Portfolio tests...${NC}"
            if [ -f "./test_portfolio" ]; then
                ./test_portfolio
                if [ $? -eq 0 ]; then
                    echo -e "${GREEN}✓ Portfolio tests passed${NC}"
                else
                    echo -e "${RED}✗ Portfolio tests failed${NC}"
                    cd ..
                    exit 1
                fi
            else
                echo -e "${RED}✗ test_portfolio not found${NC}"
                cd ..
                exit 1
            fi
            ;;
        "risk_engine")
            echo -e "${YELLOW}Running RiskEngine tests...${NC}"
            if [ -f "./test_risk_engine" ]; then
                ./test_risk_engine
                if [ $? -eq 0 ]; then
                    echo -e "${GREEN}✓ RiskEngine tests passed${NC}"
                else
                    echo -e "${RED}✗ RiskEngine tests failed${NC}"
                    cd ..
                    exit 1
                fi
            else
                echo -e "${RED}✗ test_risk_engine not found${NC}"
                cd ..
                exit 1
            fi
            ;;
        *)
            echo -e "${RED}✗ Unknown test: $TEST_NAME${NC}"
            echo "Available tests: all, blackscholes, portfolio, risk_engine"
            cd ..
            exit 1
            ;;
    esac
    
    cd ..
}

# Parse arguments - DEFAULT to clean and build if no args
if [ $# -eq 0 ]; then
    echo -e "${YELLOW}No options provided - running clean and build (use --help for options)${NC}"
    clean
    build
    exit 0
fi

# Track if we're doing clean-only or compiler-only
CLEAN_ONLY=false
COMPILER_ONLY=false
if [ $# -eq 1 ] && [ "$1" == "--clean" ]; then
    CLEAN_ONLY=true
fi
if [ $# -eq 2 ] && [ "$1" == "--compiler" ]; then
    COMPILER_ONLY=true
fi

# Track if any action was taken
ACTION_TAKEN=false

while [ $# -gt 0 ]; do
    case "$1" in
        --help)
            show_help
            exit 0
            ;;
        --clean)
            clean
            ACTION_TAKEN=true
            if [ "$CLEAN_ONLY" = true ]; then
                exit 0
            fi
            shift
            ;;
        --compiler)
            if [ "$2" == "gcc" ]; then
                COMPILER="g++"
            elif [ "$2" == "clang" ]; then
                COMPILER="clang++"
            else
                echo -e "${RED}✗ Invalid compiler: $2${NC}"
                echo "Use 'gcc' or 'clang'"
                exit 1
            fi
            shift 2
            ;;
        --build)
            build
            ACTION_TAKEN=true
            shift
            ;;
        --test-all)
            run_tests "all"
            ACTION_TAKEN=true
            shift
            ;;
        --test)
            if [ -z "$2" ] || [[ "$2" == --* ]]; then
                # No test name provided, run all tests
                run_tests "all"
                shift
            else
                # Specific test name provided
                run_tests "$2"
                shift 2
            fi
            ACTION_TAKEN=true
            ;;
        --all)
            clean
            build
            run_tests "all"
            ACTION_TAKEN=true
            shift
            ;;
        *)
            echo -e "${RED}✗ Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

# If only compiler was specified, run default clean and build
if [ "$COMPILER_ONLY" = true ]; then
    echo -e "${YELLOW}Compiler set to $COMPILER - running clean and build${NC}"
    clean
    build
fi
