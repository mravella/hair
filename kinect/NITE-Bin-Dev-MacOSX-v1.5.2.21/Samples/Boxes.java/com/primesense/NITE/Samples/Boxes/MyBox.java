package com.primesense.NITE.Samples.Boxes;

import java.awt.Color;
import java.awt.Component;

import org.OpenNI.GeneralException;
import org.OpenNI.IObservable;
import org.OpenNI.IObserver;
import org.OpenNI.StatusException;

import com.primesense.NITE.*;

class LeaveEvent extends java.util.Observable
{
	void raise()
	{
		setChanged();
		notifyObservers();
	}
}


public class MyBox extends PointControl
{
	public MyBox(String name, Component panel) throws GeneralException
	{
		super();
		this._name = name;
		this._panel = panel;
		
		pushDetector = new PushDetector();
		swipeDetector = new SwipeDetector();
		steadyDetector = new SteadyDetector();
		flowRouter = new FlowRouter();
		broadcaster = new Broadcaster();
		
		broadcaster.addListener(pushDetector);
		broadcaster.addListener(flowRouter);
		
		
		pushDetector.getPushEvent().addObserver(new IObserver<VelocityAngleEventArgs>()
		{
			@Override
			public void update(IObservable<VelocityAngleEventArgs> observable,
					VelocityAngleEventArgs args)
			{
				leaveEvent.raise();
			}
		});
		steadyDetector.getSteadyEvent().addObserver(new IObserver<IdValueEventArgs>()
		{
			@Override
			public void update(IObservable<IdValueEventArgs> observable,
					IdValueEventArgs args)
			{
				System.out.println("Box " + _name + ": Steady");
				try {
					if (args.getId() == getPrimaryID())
					{
						try {
							flowRouter.setActive(swipeDetector);
						} catch (StatusException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						ChangeColor(Color.WHITE);
					}
				} catch (StatusException e) {
					e.printStackTrace();
				}
			}
		});
		swipeDetector.getGeneralSwipeEvent().addObserver(new IObserver<DirectionVelocityAngleEventArgs>()
		{
			@Override
			public void update(IObservable<DirectionVelocityAngleEventArgs> observable,
					DirectionVelocityAngleEventArgs args)
			{
				System.out.println("Box " + _name + ": Swipe " + args.getDirection());
				try {
					flowRouter.setActive(steadyDetector);
				} catch (StatusException e) {
					e.printStackTrace();
				}
				ChangeColor(Color.RED);
			}
		});
		
		getPrimaryPointCreateEvent().addObserver(new IObserver<HandPointEventArgs>()
		{
			@Override
			public void update(IObservable<HandPointEventArgs> observable,
					HandPointEventArgs args)
			{
				try {
					flowRouter.setActive(steadyDetector);
				} catch (StatusException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				// TODO Auto-generated method stub
				
			}
		});
		getPrimaryPointDestroyEvent().addObserver(new IObserver<IdEventArgs>()
		{
			@Override
			public void update(IObservable<IdEventArgs> observable, IdEventArgs args)
			{
				ChangeColor(Color.CYAN);
			}
		});
		getMessageUpdateEvent().addObserver(new IObserver<PointerEventArgs>()
		{
			@Override
			public void update(IObservable<PointerEventArgs> observable,
					PointerEventArgs args)
			{
				try {
					broadcaster.updateMessage(args.getPointer());
				} catch (StatusException e) {
					e.printStackTrace();
				}
			}
		});
		leaveEvent = new LeaveEvent();
	}

	public void ChangeColor(Color color)
	{
		_panel.setBackground(color);
	}
	
	private PushDetector pushDetector;
	private SwipeDetector swipeDetector;
	private SteadyDetector steadyDetector;
	private FlowRouter flowRouter;
	private Broadcaster broadcaster;

	private final String _name;
	private Component _panel;
	
	// Events
	public java.util.Observable getLeaveEvent()
	{
		return leaveEvent;
	}
	
	private LeaveEvent leaveEvent;
}
