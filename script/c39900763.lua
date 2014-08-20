--異次元の邂逅
function c39900763.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c39900763.target)
	e1:SetOperation(c39900763.operation)
	c:RegisterEffect(e1)
end
function c39900763.filter(c,e,tp)
	return c:IsFaceup() and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN)
end
function c39900763.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
			and Duel.IsExistingTarget(c39900763.filter,tp,LOCATION_REMOVED,0,1,nil,e,tp)
			and Duel.GetLocationCount(1-tp,LOCATION_MZONE,1-tp)>0
			and Duel.IsExistingTarget(c39900763.filter,1-tp,LOCATION_REMOVED,0,1,nil,e,1-tp)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g1=Duel.SelectTarget(tp,c39900763.filter,tp,LOCATION_REMOVED,0,1,1,nil,e,tp)
	e:SetLabelObject(g1:GetFirst())
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
	local g2=Duel.SelectTarget(1-tp,c39900763.filter,1-tp,LOCATION_REMOVED,0,1,1,nil,e,1-tp)
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g1,2,0,0)
end
function c39900763.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc1=e:GetLabelObject()
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc2=g:GetFirst()
	if tc1==tc2 then tc2=g:GetNext() end
	if tc1:IsRelateToEffect(e) then
		Duel.SpecialSummonStep(tc1,0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)
	end
	if tc2:IsRelateToEffect(e) then
		Duel.SpecialSummonStep(tc2,0,1-tp,1-tp,false,false,POS_FACEDOWN_DEFENCE)
	end
	Duel.SpecialSummonComplete()
end
