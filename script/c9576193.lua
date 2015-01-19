--モンスター・スロット
function c9576193.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE+CATEGORY_SPECIAL_SUMMON+CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c9576193.target)
	e1:SetOperation(c9576193.activate)
	c:RegisterEffect(e1)
end
function c9576193.filter1(c,tp)
	local lv=c:GetLevel()
	return lv>0 and c:IsFaceup() and Duel.IsExistingTarget(c9576193.filter2,tp,LOCATION_GRAVE,0,1,nil,lv)
end
function c9576193.filter2(c,lv)
	return c:GetLevel()==lv and c:IsAbleToRemove()
end
function c9576193.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) and Duel.IsExistingTarget(c9576193.filter1,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g1=Duel.SelectTarget(tp,c9576193.filter1,tp,LOCATION_MZONE,0,1,1,nil,tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g2=Duel.SelectTarget(tp,c9576193.filter2,tp,LOCATION_GRAVE,0,1,1,nil,g1:GetFirst():GetLevel())
	e:SetLabelObject(g1:GetFirst())
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g2,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c9576193.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc1=e:GetLabelObject()
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc2=g:GetFirst()
	if tc2==tc1 then tc2=g:GetNext() end
	if tc1:IsFacedown() or not tc1:IsRelateToEffect(e) then return end
	if not tc2:IsRelateToEffect(e) or tc2:GetLevel()~=tc1:GetLevel() or Duel.Remove(tc2,POS_FACEUP,REASON_EFFECT)==0 then return end
	Duel.BreakEffect()
	if Duel.Draw(tp,1,REASON_EFFECT)==0 then return end
	local dr=Duel.GetOperatedGroup():GetFirst()
	Duel.ConfirmCards(1-tp,dr)
	Duel.BreakEffect()
	if dr:GetLevel()==tc1:GetLevel() then
		if Duel.SpecialSummon(dr,0,tp,tp,false,false,POS_FACEUP)==0 then
			Duel.ShuffleHand(tp)
		end
	else Duel.ShuffleHand(tp) end
end
