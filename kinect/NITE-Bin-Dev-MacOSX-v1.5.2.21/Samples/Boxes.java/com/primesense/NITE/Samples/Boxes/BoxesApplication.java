package com.primesense.NITE.Samples.Boxes;

import javax.swing.JFrame;

public class BoxesApplication {
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		JFrame frame = new JFrame("Boxes");

		//2. Optional: What happens when the frame closes?
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		//4. Size the frame.
		frame.pack();

		//5. Show it.
		frame.setVisible(true);
		
		
		try {
			Boxes app = new Boxes(frame);
			app.run();
		} catch (Exception e)
		{
			e.printStackTrace();
		}
		
	}
}
