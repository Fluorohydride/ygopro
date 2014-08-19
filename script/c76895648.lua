--デンジャラスマシン TYPE－6
function c76895648.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)	
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--dice
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(76895648,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)	
	e2:SetCategory(CATEGORY_ATKCHANGE)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c76895648.condition)
	e2:SetTarget(c76895648.target)
	e2:SetOperation(c76895648.operation)
	c:RegisterEffect(e2)
end
function c76895648.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer()
end
function c76895648.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DICE,nil,0,tp,1)
end
function c76895648.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local dice=Duel.TossDice(tp,1)
	if dice==1 then
		Duel.DiscardHand(tp,nil,1,1,REASON_EFFECT+REASON_DISCARD)
	elseif dice==2 then
		Duel.DiscardHand(1-tp,nil,1,1,REASON_EFFECT+REASON_DISCARD)
	elseif dice==3 then
		Duel.Draw(tp,1,REASON_EFFECT)
	elseif dice==4 then
		Duel.Draw(1-tp,1,REASON_EFFECT)
	elseif dice==5 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectMatchingCard(tp,Card.IsDestructable,tp,0,LOCATION_MZONE,1,1,nil)
		Duel.Destroy(g,REASON_EFFECT)
	else
		Duel.Destroy(e:GetHandler(),REASON_EFFECT)
	end
end
