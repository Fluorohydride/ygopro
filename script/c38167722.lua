--闇霊術－「欲」
function c38167722.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c38167722.cost)
	e1:SetTarget(c38167722.target)
	e1:SetOperation(c38167722.activate)
	c:RegisterEffect(e1)
end
function c38167722.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsAttribute,1,nil,ATTRIBUTE_DARK) end
	local g=Duel.SelectReleaseGroup(tp,Card.IsAttribute,1,1,nil,ATTRIBUTE_DARK)
	Duel.Release(g,REASON_COST)
end
function c38167722.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,2) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(2)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,2)
end
function c38167722.cfilter(c)
	return not c:IsPublic() and c:IsType(TYPE_SPELL)
end
function c38167722.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	local g=Duel.GetMatchingGroup(c38167722.cfilter,p,0,LOCATION_HAND,nil)
	if g:GetCount()>0 and Duel.SelectYesNo(1-p,aux.Stringid(38167722,0)) then
		Duel.Hint(HINT_SELECTMSG,1-p,HINTMSG_CONFIRM)
		local sg=g:Select(1-p,1,1,nil)
		Duel.ConfirmCards(p,sg)
		Duel.ShuffleHand(1-p)
		if Duel.IsChainDisablable(0) then
			Duel.NegateEffect(0)
			return
		end
	end
	Duel.Draw(p,d,REASON_EFFECT)
end
