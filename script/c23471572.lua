--ソロモンの律法書
function c23471572.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetOperation(c23471572.activate)
	c:RegisterEffect(e1)
end
function c23471572.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,0)
	e1:SetCode(EFFECT_SKIP_SP)
	if Duel.GetTurnPlayer()==tp and Duel.GetCurrentPhase()==PHASE_STANDBY then
		e1:SetCondition(c23471572.skipcon)
		e1:SetLabel(Duel.GetTurnCount())
		e1:SetReset(RESET_PHASE+RESET_STANDBY+RESET_SELF_TURN,2)
	else
		e1:SetReset(RESET_PHASE+RESET_STANDBY+RESET_SELF_TURN)
	end
	Duel.RegisterEffect(e1,tp)
end
function c23471572.skipcon(e)
	return Duel.GetTurnCount()~=e:GetLabel()
end
