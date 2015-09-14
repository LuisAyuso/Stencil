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
  #mean <- ddply(data,algorithm~num.cores,summarise, exec.time.mean=mean(exec.time), sd=sd(exec.time))
  plot + geom_boxplot(aes_string(x="algorithm", y=measure, fill="algorithm")) + facet_grid( input.size ~ num.cores) # + geom_text(data = mean,  aes(label = exec.time.mean, x = algorithm, y = exec.time.mean+sd ))
}

plot_time_steps_vs_cores <- function(data, measure){
  plot <- ggplot(data)
  
  plot + geom_boxplot(aes_string(x="algorithm", y=measure, fill="algorithm")) + facet_grid( time.steps ~ num.cores) 
}

sumarize_times <-function(data){

  mean <- ddply(data, algorithm~num.cores~Kind, summarise, exec.time.mean=mean(exec.time), sd=sd(exec.time))

  expected <- get_expected_time(merge(data, mean))
  
  return ( merge(mean, expected) )
}

plot_algorithm_vs_cores<- function(data, measure){
  tmpdata <-data

  tmpdata <- merge(tmpdata, sumarize_times(data))
  
  plot <- ggplot(tmpdata)
  plot + facet_grid( algorithm ~ num.cores) + 
          geom_boxplot(aes_string(x="Kind", y=measure, color="Kind", fill="Kind")) + geom_text(aes(label = round(exec.time.mean,2), x = Kind, y = -1500 ))+ 
          geom_boxplot(aes_string(x="Kind", y="expected"), color="#CC0000") + geom_text(aes(label = round(expected,2), x = Kind, y=-0, color="#CC0000" )) +
          geom_text(aes(label = round(speedup,2), x = Kind, y = exec.time.mean+1000))
  
}


estimate <- function( kind, alg, numthreads, exectime, df){
    
    if(length(kind) > 1) {
      kind <- kind[1]
    }    
    if(length(alg) > 1) {
      alg <- alg[1]
    }  
    if(length(numthreads) > 1) {
      numthreads <- numthreads[1]
    }  
    if(length(exectime) > 1) {
      exectime <- exectime[1]
    }
    
    res <- df[df$Kind == kind, ]
    res <- res[res$algorithm == alg, ]
    res <- res[res$num.cores == 1, ]
    
    one_thread_time <- (res$exec.time.mean[1]) 
    
    return (one_thread_time/numthreads)
}  

compute_speedup <- function( kind, alg, numthreads, exectime, df){
  if(length(kind) > 1) {
    kind <- kind[1]
  }    
  if(length(alg) > 1) {
    alg <- alg[1]
  }  
  if(length(numthreads) > 1) {
    numthreads <- numthreads[1]
  }  
  if(length(exectime) > 1) {
    exectime <- exectime[1]
  }
  
  res <- df[df$Kind == kind, ]
  res <- res[res$algorithm == alg, ]
  res <- res[res$num.cores == 1, ]
  
  one_thread_time <- (res$exec.time.mean[1]) 
  return (one_thread_time/exectime)
}


get_expected_time <- function(df){
  global_df <<- df  
  tmp1 <-  ddply(global_df, Kind~algorithm~num.cores, summarise, expected=estimate(kind=Kind, alg=algorithm, numthreads=num.cores, exectime=exec.time, df=global_df)) 
  tmp2 <-  ddply(global_df, Kind~algorithm~num.cores, summarise, speedup=compute_speedup(kind=Kind, alg=algorithm, numthreads=num.cores, exectime=exec.time, df=global_df)) 
  #rm(global_df)
  return (merge(tmp1, tmp2))
}
