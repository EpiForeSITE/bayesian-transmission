# newCppModel Function - Summary

## Overview

A new R function `newCppModel()` has been created that constructs C++ model objects from R parameter lists and returns them wrapped in appropriate reference classes for direct interaction.

## Files Created/Modified

### New Files

1. **`R/newCppModel.R`**: Main R interface
   - `newCppModel()` - User-facing function to create C++ models
   - `getCppModelParams()` - Helper to extract parameters from C++ model objects

### Modified Files

1. **`src/runMCMC.cpp`**: Added new exported C++ function
   - `newCppModelInternal()` - Creates and wraps C++ model objects

2. **`src/Module-lognormal.cpp`**: Enhanced module exports
   - Added `MixedModel` and `MixedICP` class definitions
   - Properly exposed constructors and methods

3. **`src/wrap.h`**: Updated wrapper declarations
   - Uncommented `LogNormalICP`, `LogNormalModel`, `MixedICP`, `MixedModel`
   - Added `LinearAbxModel2` pointer declaration

4. **`src/wrap.cpp`**: Updated wrapper implementations
   - Uncommented CREATE_WRAP_REFCLASS macros for all model types
   - Added `LinearAbxModel2` wrapper

5. **`man/*.Rd`**: Auto-generated documentation files
   - `newCppModel.Rd`
   - `getCppModelParams.Rd`
   - `newCppModelInternal.Rd`

6. **`NAMESPACE`**: Auto-updated with new exports

### Test File

- **`test_newCppModel.R`**: Comprehensive test suite for all model types

## Function Usage

### newCppModel()

```r
# Create a basic log-normal model
params <- LogNormalModelParams("LogNormalModel", nstates = 2)
model <- newCppModel(params, verbose = TRUE)

# Create a linear antibiotic model
params <- LinearAbxModel(nstates = 2)
model <- newCppModel(params)

# Access model properties
insituParams <- model$InsituParams
inColParams <- model$InColParams

# Get parameter values
paramValues <- inColParams$values
```

### getCppModelParams()

```r
# Extract all parameters from a model
model <- newCppModel(LinearAbxModel())
all_params <- getCppModelParams(model)

# Access individual components
print(all_params$InCol)
print(all_params$Insitu)
```

## Supported Model Types

The function supports all major model types defined in the package:

1. **LogNormalModel**: Basic log-normal transmission model
2. **LinearAbxModel**: Linear antibiotic effects model
3. **LinearAbxModel2**: Alternative linear antibiotic model
4. **MixedModel**: Mixed acquisition model (constant + mass action)

## Return Types

Each model type is wrapped in its corresponding R reference class:

- `LogNormalModel` → `Rcpp_CppLogNormalModel`
- `LinearAbxModel` → `Rcpp_CppLinearAbxModel`
- `LinearAbxModel2` → `Rcpp_CppLinearAbxModel2`
- `MixedModel` → `Rcpp_CppMixedModel`

All inherit from `CppBasicModel` and provide access to:

### Properties
- `InsituParams` - In situ colonization parameters
- `SurveillanceTestParams` - Surveillance testing parameters
- `ClinicalTestParams` - Clinical testing parameters  
- `OutColParams` - Out-of-unit colonization parameters
- `InColParams` - In-unit colonization parameters
- `AbxParams` - Antibiotic parameters (if applicable)

### Methods
- `logLikelihood(hist)` - Calculate log likelihood for system history
- `getHistoryLinkLogLikelihoods(hist)` - Individual link likelihoods
- `forwardSimulate(...)` - Forward simulation
- `initEpisodeHistory(...)` - Initialize episode history
- `sampleEpisodes(...)` - Sample episodes
- `setAbx(...)` - Set antibiotic parameters

## Implementation Details

### Parameter Setup Flow

1. User creates parameters using constructor functions (e.g., `LinearAbxModel()`)
2. `newCppModel()` validates parameters and calls `newCppModelInternal()`
3. C++ function uses existing `newModel()` to instantiate the model
4. `modelsetup<T>()` configures all parameters from the R list
5. Model is wrapped in appropriate reference class using `CREATE_WRAP_REFCLASS` macro
6. Reference class object is returned to R

### Model Parameter Configuration

The `modelsetup<T>()` template function (in `src/modelsetup.h`) handles:
- Antibiotic settings
- In situ parameters
- Test parameters (surveillance and clinical)
- Out-of-unit infection parameters
- In-unit colonization parameters  
- Antibiotic rate parameters

Specialized versions exist for `LinearAbxModel` to handle its specific ICP type.

### Reference Class Wrapping

The `CREATE_WRAP_REFCLASS` macro in `wrap.cpp` creates template specializations that:
1. Create R reference class instances
2. Store C++ pointer in `.pointer` slot
3. Provide R-level access to C++ methods and properties

## Testing

Test script validates:
- ✓ All four model types can be created
- ✓ Correct reference classes are returned
- ✓ Properties are accessible
- ✓ Parameters can be extracted
- ✓ Error handling for invalid model names

## Benefits

1. **Direct C++ Access**: Work with C++ models without running full MCMC
2. **Type Safety**: Proper reference classes for each model type
3. **Method Access**: Call C++ methods directly from R
4. **Parameter Inspection**: Extract and examine model parameters
5. **Testing**: Validate model setup and calculations independently
6. **Debugging**: Inspect model state at any point

## Usage Examples

### Inspect Initial Likelihood

```r
params <- LinearAbxModel(nstates = 2)
model <- newCppModel(params)

# Create system and history (requires data)
sys <- # ... create system from data
hist <- # ... create history

# Calculate likelihood
ll <- model$logLikelihood(hist)
```

### Verify Parameter Settings

```r
params <- LogNormalModelParams("LogNormalModel", nstates = 2)
model <- newCppModel(params, verbose = TRUE)

# Extract and verify
all_params <- getCppModelParams(model)
stopifnot(all_params$Insitu[1] == 0.9)  # Check uncolonized prob
```

### Compare Models

```r
model1 <- newCppModel(LogNormalModelParams("LogNormalModel"))
model2 <- newCppModel(LinearAbxModel())

# Compare parameter structures
params1 <- getCppModelParams(model1)
params2 <- getCppModelParams(model2)
```

## Future Enhancements

Potential additions:
- Support for system/history creation from model
- Parameter update methods
- Model comparison utilities
- Serialization/deserialization
- More comprehensive parameter extraction options
