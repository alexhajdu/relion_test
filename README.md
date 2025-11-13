# ReLion test

## Intro
Thanks for the opportunity to work on this task. It was quite stressful TBH and noted to say there are many other options to accomplish it in a different or/and better way. I chose this one, there was no time to change, so here we go :)

### Notes:
- See ///@note in the code - I have some thoughts in the code
- I chose multiplayer implementation over visual fidelity. Sorry if your eyes hurt :D
- No time for animations - using timeline for animating transform properties
- I had issues packaging on my current Mac computer, so I hope this repo is enough. I can showcase if needed.
- Organization is much messier than it would be normally

## Result
- ![Nov-12-2025 12-43-11](https://github.com/user-attachments/assets/7e5de939-0224-438a-a27d-5d3e74034eab)

---

## Approach

### IREInteractableInterface
One interface to rule them all. I reused the same interface for both levers and objects that could be interacted with by the lever. 
In a way, both represent interactable objects from the programming POV, even though one looks only as a visual representation.
I consider it fine and generic, especially for this time limited example.

### ARELeverActor
Implements the `IREInteractableInterface`, and it's the lever that actor designers can create in BP. There are some basic config exposed (length, ..). Customization is also achieved via exposed components' props (SM, sound)

### ARELightActor && BP_Elevator
Both are implementing `IREInteractableInterface`, but one is done in C++, and one to show flexibility, created completely in the Blueprints.

## For designers

### Lever
- Create BP from <img width="726" height="632" alt="Screenshot 2025-11-12 at 12 24 19" src="https://github.com/user-attachments/assets/462833ba-e123-4649-9bb3-5192256e7808" />
- Use this properties to define connected actor and basic appearance setup. <img width="639" height="222" alt="Screenshot 2025-11-12 at 12 25 38" src="https://github.com/user-attachments/assets/eb9afaa1-2738-40a7-be85-efeb61932951" /> For setting SM or sound use settings on the corresponding components.
- If there is no actor associated with the lever, you will see TRex. Otherwise, the line will be presented to see what actor is assigned for the lever.
- ![Nov-13-2025 12-00-03](https://github.com/user-attachments/assets/a111ad4c-8699-4eb2-aa97-317533e29629)

### Interactables
- Create Actor BP and implement interface
- <img width="640" height="118" alt="Screenshot 2025-11-12 at 12 31 19" src="https://github.com/user-attachments/assets/4c7c9ccd-379c-4a59-9048-1d5b09010015" /><img width="637" height="338" alt="Screenshot 2025-11-13 at 09 47 52" src="https://github.com/user-attachments/assets/7dca78e2-d5bd-483a-91b9-89e849f81769" />
- Then this actor, once placed in the world and attached to the lever as `ConnectedActor` will do the job for you.
