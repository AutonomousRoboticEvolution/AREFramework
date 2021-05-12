# LC_NSMS

## How to load body plans?

You need to set these three parameters. 

```
#isBootstrapEvolution,bool,1
#loadExperiment,string,/file/path
#bootstrapFile,string,filename.csv
```

*If you set the first parameter to true, the experiment is going to use a bootstrap population
*In the second parameter you specify the filepath to the bootstrap population directory
*The third parameter specifies the name of the file of the list of the robots. There are two important remarks about this file. The list of robots need to be in a single column. You need at least N body plans where N is the size of the bootstrap population.
