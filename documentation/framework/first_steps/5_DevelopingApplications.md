[Previous Topic](4_RunningFreiTest.md) - [Index](../../../README.md)

# Applications

The framework consists of a collection of various applications.
Each application represents a workflow which is targeted for one specific use case.
Each application can have specific configuration options.

A basic workflow roughly follows these steps:

1. Load the configuration from the command line
2. Load the configuration from the configuration file
3. Load the Circuit from the source files
   1. Load the cell library sources
   2. Load the circuit sources
4. Run the application-specific steps (user-defined)
   - Encode the circuit and apply SAT / BMC solving under constraints
   - Run fault simulations
   - Run external tools
   - Import or export files
5. Export statistics

## Developing Applications

The framework provides the basic building blocks to create your own workflows.
To benefit from the argument parsing and circuit initialization, create a C++ class that inherits from the BaseApplication class.
There are multiple methods that can be overridden (e.g. Init and Run methods) to adjust the behaviour of the workflow.

```cpp
using namespace FreiTest::Application;

class MyWorkflow: public BaseApplication
{
public:
	MyWorkflow(void) = default;
	virtual ~MyWorkflow(void) = default;

	void Init(void) override
	{
		LOG(INFO) << "My new workflow has been initialized";
	}

	void Run(void) override
	{
		LOG(INFO) << "My new workflow has been executed";
	}
};
```

To integrate the workflow into the framework add the workflow to the BaseApplication Create method like shown below:

```cpp
unique_ptr<BaseApplication> BaseApplication::Create(std::string application)
{
	...
	if (application == "MyProject/MyWorkflow")
		return std::make_unique<MyWorkflow>();

	return std::unique_ptr<BaseApplication>();
}
```

# Settings

Custom configuration options are defined by overriding the `SetSetting` method.
The implementation of the method receives the key-value pairs of each configuration option.
Only configuration options that are not in the global list of configuration options are passed to the method.
Return false from the method to signal an invalid configuration option.

```cpp
using namespace FreiTest::Application;

class MyWorkflow: public BaseApplication
{
public:
	MyWorkflow(void) = default;
	virtual ~MyWorkflow(void) = default;

	void Run(void) override
	{
		LOG(INFO) << "Option ConfigOption is " << myConfigOption;
	}

    bool SetSetting(std::string key, std::string value) override
	{
		if (key == "MyProject/MyWorkflow/ConfigOption")
		{
			myConfigOption = value;
			return true;
		}

		return false;
	}

private:
	std::string myConfigOption;
};
```

# Statistics

Custom statistics can be created by overriding the `GetStatistics` method in a workflow.
The returned statistics instance contains the statistics of the workflow and is merged with the frameworks statistics.

Statistics are added with a name of the entry, a raw value (uint64_t, int64_t, uint32_t, int32_t, double, float or string), unit and a description for documentation purposes.
By using dots the exported values can be grouped (nested groups are possible).
This information will then be exported as JSON data in the statistics output file.

```cpp
using namespace FreiTest::Application;

class MyWorkflow: public BaseApplication
{
public:
	MyWorkflow(void) = default;
	virtual ~MyWorkflow(void) = default;

	void Run(void) override
	{
		statistics.Add("TimeSpentDoingNothing", 10u, "Second(s)",
			"The time that was spent by this workflow waiting");
		statistics.Add("TimeSpentDoingSomething", 0u, "Second(s)",
			"The time that was used for processing data");

		statistics.Add("MyGroup.MyNestedValue", 10u, "Unit(s)",
			"First statistic to show the use of grouping");
		statistics.Add("MyGroup.MyNestedValue2", 20u, "Unit(s)",
			"Second statistic to show the use of grouping");
	}

    ApplicationStatistics GetStatistics(void) override
	{
		return statistics;
	}

private:
    ApplicationStatistics statistics;
};
```

Resulting data (json):

```xml
{
    "FreiTest": {
        ...
    },
    "Options": {
        ...
    },
    "Circuit": {
        ...
    },
    "Application": {
        "TimeSpentDoingNothing" {
            "Value": 10,
            "Unit": "Second(s)",
            "Description": "The time that was spent by this workflow waiting"
        },
        "TimeSpentDoingSomething": {
            "Value": 0,
            "Unit": "Second(s)",
            "Description": "The time that was used for processing data"
        },
        "MyGroup": {
            "MyNestedValue": {
                "Value": 10,
                "Unit": "Unit(s)",
                "Description": "First statistic to show the use of grouping"
            },
            "MyNestedValue2": {
                "Value": 0,
                "Unit": "Unit(s)",
                "Description": "Second statistic to show the use of grouping"
            }
        }
    }
}
```

[Previous Topic](4_RunningFreiTest.md) - [Index](../../../README.md)
