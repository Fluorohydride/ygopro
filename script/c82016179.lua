--森羅の施し
function c82016179.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW+CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,82016179+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c82016179.target)
	e1:SetOperation(c82016179.activate)
	c:RegisterEffect(e1)
end
function c82016179.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,3) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(3)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,3)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,0,tp,2)
end
function c82016179.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	if Duel.Draw(p,d,REASON_EFFECT)==3 then
		Duel.ShuffleHand(p)
		Duel.BreakEffect()
		local g=Duel.GetMatchingGroup(Card.IsAbleToDeck,p,LOCATION_HAND,0,nil)
		if g:GetCount()>1 and g:IsExists(Card.IsSetCard,1,nil,0x90) then
			Duel.Hint(HINT_SELECTMSG,p,HINTMSG_TODECK)
			local sg1=g:FilterSelect(p,Card.IsSetCard,1,1,nil,0x90)
			Duel.Hint(HINT_SELECTMSG,p,HINTMSG_TODECK)
			local sg2=g:Select(p,1,1,sg1:GetFirst())
			sg1:Merge(sg2)
			Duel.ConfirmCards(1-p,sg1)
			Duel.SendtoDeck(sg1,nil,0,REASON_EFFECT)
			Duel.SortDecktop(p,p,2)
		else
			local hg=Duel.GetFieldGroup(p,LOCATION_HAND,0)
			Duel.ConfirmCards(1-p,hg)
			local ct=Duel.SendtoDeck(hg,nil,0,REASON_EFFECT)
			Duel.SortDecktop(p,p,ct)
		end
	end
end
