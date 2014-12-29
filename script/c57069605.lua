--魂の氷結
function c57069605.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_MAIN_END+TIMING_DRAW_PHASE)
	e1:SetCondition(c57069605.condition)
	e1:SetOperation(c57069605.activate)
	c:RegisterEffect(e1)
end
function c57069605.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetLP(tp)+2000<=Duel.GetLP(1-tp)
end
function c57069605.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SKIP_BP)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(0,1)
	if Duel.GetTurnPlayer()~=tp and Duel.GetCurrentPhase()==PHASE_BATTLE then
		e1:SetLabel(Duel.GetTurnCount())
		e1:SetCondition(c57069605.skipcon)
		e1:SetReset(RESET_PHASE+PHASE_BATTLE+RESET_OPPO_TURN,2)
	else
		e1:SetReset(RESET_PHASE+PHASE_BATTLE+RESET_OPPO_TURN,1)
	end
	Duel.RegisterEffect(e1,tp)
end
function c57069605.skipcon(e)
	return Duel.GetTurnCount()~=e:GetLabel()
end
