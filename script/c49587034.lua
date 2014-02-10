--光の封札剣
function c49587034.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetTarget(c49587034.target)
	e1:SetOperation(c49587034.activate)
	c:RegisterEffect(e1)
end
function c49587034.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToRemove,tp,0,LOCATION_HAND,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,1,1-tp,LOCATION_HAND)
end
function c49587034.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(1-tp,LOCATION_HAND,0,nil)
	local rs=g:RandomSelect(1-tp,1)
	local card=rs:GetFirst()
	if card==nil then return end
	if Duel.Remove(card,POS_FACEDOWN,REASON_EFFECT)>0 then
		local ph=Duel.GetCurrentPhase()
		local cp=Duel.GetTurnPlayer()
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetRange(LOCATION_REMOVED)
		e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
		e1:SetCountLimit(1)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_STANDBY+RESET_SELF_TURN,4)
		e1:SetCondition(c49587034.thcon)
		e1:SetOperation(c49587034.thop)
		e1:SetLabel(1)
		card:RegisterEffect(e1)
		e:GetHandler():RegisterFlagEffect(1082946,RESET_PHASE+PHASE_END+RESET_SELF_TURN,0,3)
		c49587034[e:GetHandler()]=e1
	end
end
function c49587034.thcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c49587034.thop(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	e:GetOwner():SetTurnCounter(ct)
	if ct==4 then
		Duel.SendtoHand(e:GetHandler(),nil,REASON_EFFECT)
		e:GetOwner():ResetFlagEffect(1082946)
	else
		e:SetLabel(ct+1)
	end
end
