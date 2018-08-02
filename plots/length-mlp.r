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
plot(range(1,7), range(0.605, 0.765), type="n", ylab = "F1-Measure", xlab = "# of next or prior words/punctuation marks in a sample (l)")
lines(data$n, data$fmeasure, lwd=1.5, lty=1, type='b', col='blue', pch=18)
points(x = 2, y = 0.760, col = "red", pch = 1, cex = 2)
textxy(data$n, data$fmeasure, labs=data$fmeasure, pos=3,  cex = 0.7)

head(data)
