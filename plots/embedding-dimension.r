#library
library(ggplot2)
library(reshape2)
library(pROC)
library(calibrate)

# melt data
wd = '/home/mehedi/teana/projects/eCoaching-Text-Segmentation/plots/'

# melt data
# print AUC values
data <- read.csv(paste(wd,"parameters/embedding-dimension.csv", sep=''))

# plot graph
plot(range(50,320), range(0.798, 0.826), type="n", ylab = "F1-Measure", xlab = "# of word embedding dimensions")
lines(data$dim, data$pf1, lwd=1.5, lty=1, type='b', col='green', pch=18)
points(x = 300, y = 0.825, col = "red", pch = 18, cex = 1, lwd=1)
points(x = 300, y = 0.825, col = "red", pch = 1, cex = 1.5)
text(300, 0.825, labels = c('0.825'), pos = 1, cex = 0.7)
textxy(data$dim, data$pf1, labs=data$pf1, pos=3,  cex = 0.7)

lines(data$dim, data$tf1, lwd=1.5, lty=1, type='b', col='blue', pch=17)
points(x = 300, y = 0.816, col = "blueviolet", pch = 17, cex = 1, lwd=1)
text(300, 0.816, labels = c('0.816'), pos = 1, cex = 0.7)
textxy(data$dim, data$tf1, labs=data$tf1, pos=1,  cex = 0.7)

legend(222, 0.807, c("trained GloVe", "pre-trained GloVe", "trained word2vec", "pre-trained word2vec"), 
       lty=c(1,1,1), col=c('blue', 'green', 'blueviolet', 'red'), cex=1, lwd=1.5)

head(data)