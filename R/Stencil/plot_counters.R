
plot_measure <- function(data, measure){
  
  newdata <- data
  newdata$class <- factor(data$algorithm)
  
  plot <- ggplot(newdata)
  
  
  #plot +  geom_boxplot(aes_string(x="time.steps", y=measure, fill="class")) +scale_y_log10()
  
  plot + facet_grid( ~ time.steps) + geom_boxplot(aes_string(x="class", y=measure   )) +scale_y_log10() 
}