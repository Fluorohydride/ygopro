--紅蓮の指名者
function c43262273.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_TOHAND)
	e1:SetCost(c43262273.cost)
	e1:SetTarget(c43262273.target)
	e1:SetOperation(c43262273.activate)
	c:RegisterEffect(e1)
end
function c43262273.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,2000) and Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)~=0
		and not Duel.IsExistingMatchingCard(Card.IsPublic,tp,LOCATION_HAND,0,1,nil) end
	Duel.PayLPCost(tp,2000)
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	Duel.ConfirmCards(1-tp,g)
	Duel.ShuffleHand(tp)
end
function c43262273.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)~=0
		and Duel.IsExistingMatchingCard(Card.IsAbleToRemove,tp,0,LOCATION_HAND,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,0,1-tp,LOCATION_HAND)
end
function c43262273.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,0,LOCATION_HAND,nil)
	if g:GetCount()>0 then
		Duel.ConfirmCards(tp,g)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local sg=g:Select(tp,1,1,nil)
		Duel.Remove(sg,POS_FACEUP,REASON_EFFECT)
		Duel.ShuffleHand(1-tp)
		local tc=sg:GetFirst()
		tc:RegisterFlagEffect(43262273,RESET_EVENT+0x1fe0000,0,0)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetCondition(c43262273.retcon)
		e1:SetOperation(c43262273.retop)
		e1:SetLabel(Duel.GetTurnCount())
		e1:SetLabelObject(tc)
		Duel.RegisterEffect(e1,tp)
	end
end
function c43262273.retcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and Duel.GetTurnCount()~=e:GetLabel()
end
function c43262273.retop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if tc:GetFlagEffect(43262273)~=0 then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
	e:Reset()
end
