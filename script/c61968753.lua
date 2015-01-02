--バブル・シャッフル
function c61968753.initial_effect(c)
	--pos
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c61968753.postg)
	e1:SetOperation(c61968753.posop)
	c:RegisterEffect(e1)
end
function c61968753.filter1(c)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsCode(79979666)
end
function c61968753.filter2(c)
	return c:IsPosition(POS_FACEUP_ATTACK)
end
function c61968753.spfilter(c,e,tp)
	return c:IsSetCard(0x3008) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c61968753.postg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c61968753.filter1,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingTarget(c61968753.filter2,tp,0,LOCATION_MZONE,1,nil)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c61968753.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_POSCHANGE)
	local g1=Duel.SelectTarget(tp,c61968753.filter1,tp,LOCATION_MZONE,0,1,1,nil)
	e:SetLabelObject(g1:GetFirst())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_POSCHANGE)
	local g2=Duel.SelectTarget(tp,c61968753.filter2,tp,0,LOCATION_MZONE,1,1,nil)
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g1,2,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c61968753.pfilter(c,e)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsRelateToEffect(e)
end
function c61968753.posop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(c61968753.pfilter,nil,e)
	if g:GetCount()==2 then
		Duel.ChangePosition(g,POS_FACEUP_DEFENCE)
		local tc=e:GetLabelObject()
		if Duel.GetLocationCount(tp,LOCATION_MZONE)>-1 and not tc:IsImmuneToEffect(e) and tc:IsReleasable() then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local sg=Duel.SelectMatchingCard(tp,c61968753.spfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
			Duel.Release(tc,REASON_EFFECT)
			Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
