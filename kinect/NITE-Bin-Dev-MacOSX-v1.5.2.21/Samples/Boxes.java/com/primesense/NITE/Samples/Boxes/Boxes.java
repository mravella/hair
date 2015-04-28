package com.primesense.NITE.Samples.Boxes;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JPanel;
import javax.swing.JFrame;

import org.OpenNI.*;

import com.primesense.NITE.*;
import com.primesense.NITE.Direction;

public class Boxes {

	JFrame frame;

	JPanel boxesPanel;
	JPanel panel1;
	JPanel panel2;
	JPanel panel3;
	JPanel sliderPanel;
	JPanel sliderCursor;

	public Boxes(JFrame frame)
	{
		this.frame = frame;
		boxesPanel = new JPanel();
		panel1 = new JPanel();
		panel2 = new JPanel();
		panel3 = new JPanel();
		panel1.setPreferredSize(new Dimension(200, 75));
		panel2.setPreferredSize(new Dimension(200, 75));
		panel3.setPreferredSize(new Dimension(200, 75));

		sliderPanel = new JPanel();
		sliderPanel.setPreferredSize(new Dimension(600, 20));
		sliderPanel.setBackground(Color.GRAY);

		sliderCursor = new JPanel();

		sliderCursor.setBounds(300, 85, 6, 20);
		sliderCursor.setPreferredSize(new Dimension(6, 10));
		sliderCursor.setBackground(Color.ORANGE);

		boxesPanel.add(panel1, BorderLayout.WEST);
		boxesPanel.add(panel2, BorderLayout.CENTER);
		boxesPanel.add(panel3, BorderLayout.EAST);


		frame.add(sliderCursor);
		frame.getContentPane().add(sliderPanel, BorderLayout.SOUTH);
		frame.getContentPane().add(boxesPanel);


		frame.pack();

		frame.addKeyListener(new KeyListener()
		{
			@Override
			public void keyTyped(KeyEvent arg0) {}
			@Override
			public void keyReleased(KeyEvent arg0) {}
			@Override
			public void keyPressed(KeyEvent arg0) {
				if (arg0.getKeyCode() == KeyEvent.VK_ESCAPE)
				{
					shouldRun = false;
				}
			}
		});
		
		try {
			scriptNode = new OutArg<ScriptNode>();
			this.context = Context.createFromXmlFile(SAMPLE_XML_FILE, scriptNode);
			this.sessionManager = new SessionManager(this.context, new String("Click"));
			this.flowRouter = new FlowRouter();
			this.selectableSlider = new SelectableSlider1D(3, Axis.X);
			this.boxes = new MyBox[3];
			this.boxes[0] = new MyBox("Box1", panel1);
			this.boxes[1] = new MyBox("Box2", panel2);
			this.boxes[2] = new MyBox("Box3", panel3);

			boxesObserver = new Observer()
			{
				public void update(Observable o, Object arg)
				{
					try {
						flowRouter.setActive(selectableSlider);
					} catch (StatusException e) {
						e.printStackTrace();
					}
				}
			};
			
			this.boxes[0].getLeaveEvent().addObserver(boxesObserver);
			this.boxes[1].getLeaveEvent().addObserver(boxesObserver);
			this.boxes[2].getLeaveEvent().addObserver(boxesObserver);
			
			this.selectableSlider.getItemHoverEvent().addObserver(new IObserver<IndexEventArgs>()
			{
				@Override
				public void update(IObservable<IndexEventArgs> observable,
						IndexEventArgs args)
				{
					boxes[0].ChangeColor(Color.CYAN);
					boxes[1].ChangeColor(Color.CYAN);
					boxes[2].ChangeColor(Color.CYAN);
					boxes[args.getIndex()].ChangeColor(Color.YELLOW);
				}
			});

			this.selectableSlider.getItemSelectEvent().addObserver(new IObserver<IndexDirectionEventArgs>()
			{
				@Override
				public void update(IObservable<IndexDirectionEventArgs> observable,
						IndexDirectionEventArgs args)
				{
					if (args.getDirection() == Direction.UP)
					{
						try {
							flowRouter.setActive(boxes[args.getIndex()]);
						} catch (StatusException e) {
							e.printStackTrace();
						}
					}
				}
			});
			
			this.selectableSlider.getValueChangeEvent().addObserver(new IObserver<ValueEventArgs>()
			{
				@Override
				public void update(IObservable<ValueEventArgs> observable,
						ValueEventArgs args) {
					sliderCursor.setBounds((int)(args.getValue()*600), 85, 6, 20);
				}
			});
			
			this.selectableSlider.getPrimaryPointCreateEvent().addObserver(new IObserver<HandPointEventArgs>()
			{
				@Override
				public void update(IObservable<HandPointEventArgs> observable,
						HandPointEventArgs args)
				{
					sliderPanel.setBackground(Color.RED);
				}
			});
			
			this.selectableSlider.getPrimaryPointDestroyEvent().addObserver(new IObserver<IdEventArgs>()
			{
				@Override
				public void update(IObservable<IdEventArgs> observable, IdEventArgs args)
				{
					sliderPanel.setBackground(Color.WHITE);
				}
			});
			this.sessionManager.getSessionStartEvent().addObserver(new IObserver<PointEventArgs>()
			{
				@Override
				public void update(IObservable<PointEventArgs> observable,
						PointEventArgs args)
				{
					System.out.println("Session Start");
					try {
						flowRouter.setActive(selectableSlider);
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
			});
			
			this.sessionManager.addListener(this.flowRouter);
			this.shouldRun = true;
			
		} catch (GeneralException e) {
			e.printStackTrace();
		}
	}
	
	void run()
	{
		while (shouldRun)
		{
			try {
				this.context.waitAnyUpdateAll();
				this.sessionManager.update(this.context);
			} catch (StatusException e) {
				e.printStackTrace();
				break;
			}
		}
		this.frame.dispose();
	}
	
	
    private final String SAMPLE_XML_FILE = "../../../Data/Sample-Tracking.xml";

    private Context context;
    private OutArg<ScriptNode> scriptNode;
    
    private boolean shouldRun;
    private SessionManager sessionManager;
    private FlowRouter flowRouter;
    private SelectableSlider1D selectableSlider;
    private MyBox[] boxes;
    
    private Observer boxesObserver;
}
