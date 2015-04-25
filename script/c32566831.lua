--紅玉の宝札
function c32566831.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCountLimit(1,32566831+EFFECT_COUNT_CODE_OATH)
	e1:SetCost(c32566831.cost)
	e1:SetTarget(c32566831.target)
	e1:SetOperation(c32566831.activate)
	c:RegisterEffect(e1)
end
function c32566831.cfilter(c)
	return c:IsSetCard(0x3b) and c:GetLevel()==7 and c:IsAbleToGraveAsCost()
end
function c32566831.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c32566831.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c32566831.cfilter,1,1,REASON_COST,nil)
end
function c32566831.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,2) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(2)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,2)
end
function c32566831.tgfilter(c)
	return c:IsSetCard(0x3b) and c:GetLevel()==7 and c:IsAbleToGrave()
end
function c32566831.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	local dr=Duel.Draw(p,d,REASON_EFFECT)
	local g=Duel.GetMatchingGroup(c32566831.tgfilter,p,LOCATION_DECK,0,nil)
	if dr~=0 and g:GetCount()>0 and Duel.SelectYesNo(p,aux.Stringid(32566831,0)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,p,HINTMSG_TOGRAVE)
		local sg=g:Select(p,1,1,nil)
		Duel.SendtoGrave(sg,REASON_EFFECT)
	end
end
