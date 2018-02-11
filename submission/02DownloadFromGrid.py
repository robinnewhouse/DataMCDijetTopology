#!/usr/bin/env python
import TopExamples.grid

scope          = 'user.rnewhous'
datasetPattern = '*02-06-02*output.root'
directory      = '/tmp/newhouse/DownloadFolder/'

TopExamples.grid.download(scope, datasetPattern, directory)
