#library
library(ggplot2)
library(reshape2)
library(pROC)
library(calibrate)

# melt data
wd = '/home/mehedi/teana/projects/eCoaching-Text-Segmentation/plots/'

# melt data
# print AUC values
data <- read.csv(paste(wd,"parameters/length.csv", sep=''))

# plot graph
plot(range(2,14), range(0.605, 0.765), type="n", ylab = "F1-Measure", xlab = "sliding window size (2n)")
lines(data$n, data$fmeasure, lwd=1.5, lty=1, type='b', col='blue', pch=18)
points(x = 4, y = 0.760, col = "red", pch = 1, cex = 2)
textxy(data$n, data$fmeasure, labs=data$fmeasure, pos=3,  cex = 0.7)

head(data)
