--森羅の施し
function c82016179.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c82016179.cost)
	e1:SetTarget(c82016179.target)
	e1:SetOperation(c82016179.activate)
	c:RegisterEffect(e1)
end
function c82016179.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,82016179)==0 end
	Duel.RegisterFlagEffect(tp,82016179,RESET_PHASE+PHASE_END,0,1)
end
function c82016179.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,3) end
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,3)
end
function c82016179.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Draw(tp,3,REASON_EFFECT)
	Duel.BreakEffect()
	local g=Duel.GetMatchingGroup(Card.IsAbleToDeck,tp,LOCATION_HAND,0,nil)
	if g:GetCount()>1 and g:IsExists(Card.IsSetCard,1,nil,0x90) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		local sg1=g:FilterSelect(tp,Card.IsSetCard,1,1,nil,0x90)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		local sg2=g:Select(tp,1,1,sg1:GetFirst())
		sg1:Merge(sg2)
		Duel.ConfirmCards(1-tp,sg1)
		Duel.SendtoDeck(sg1,nil,0,REASON_EFFECT)
		Duel.SortDecktop(tp,tp,2)
	else
		local hg=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
		Duel.ConfirmCards(1-tp,hg)
		local ct=Duel.SendtoDeck(hg,nil,0,REASON_EFFECT)
		Duel.SortDecktop(tp,tp,ct)
	end
end
