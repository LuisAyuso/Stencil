require("ggplot2")

plot_measure <- function(data, measure){
  
  newdata <- data
  newdata$class <- factor(data$algorithm)
  
  plot <- ggplot(newdata)
  
  
  #plot +  geom_boxplot(aes_string(x="time.steps", y=measure, fill="class")) +scale_y_log10()
  
  plot + facet_grid( ~ time.steps) + geom_boxplot(aes_string(x="class", y=measure   )) +scale_y_log10() 
}


plot_input_size_vs_cores <- function(data, measure){
  plot <- ggplot(data)
  plot + geom_boxplot(aes_string(x="algorithm", y=measure)) + facet_grid( input.size ~ num.cores)
}


plot_bars_avg <- function( data, measure){

 #meanMes <- tapply(data[measure], diamonds$cut, mean)
}
