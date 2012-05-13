--ミラクル・コンタクト
function c35255456.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c35255456.target)
	e1:SetOperation(c35255456.activate)
	c:RegisterEffect(e1)
end
function c35255456.filter1(c,e)
	return c:IsAbleToDeck() and c:IsCanBeFusionMaterial() and not c:IsImmuneToEffect(e) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c35255456.filter2(c,e,tp,m,chkf)
	return c:IsType(TYPE_FUSION) and c:IsSetCard(0x3008) and c:IsSetCard(0x9) and c:GetCode()~=31111109 
		and c:IsCanBeSpecialSummoned(e,0,tp,true,false) and c:CheckFusionMaterial(m,nil,chkf)
end
function c35255456.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local chkf=Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and PLAYER_NONE or tp
		local mg=Duel.GetMatchingGroup(c35255456.filter1,tp,LOCATION_HAND+LOCATION_GRAVE+LOCATION_MZONE,0,nil,e)
		return Duel.IsExistingMatchingCard(c35255456.filter2,tp,LOCATION_EXTRA,0,1,nil,e,tp,mg,chkf)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c35255456.cffilter(c)
	return c:IsLocation(LOCATION_HAND) or (c:IsLocation(LOCATION_MZONE) and c:IsFacedown())
end
function c35255456.activate(e,tp,eg,ep,ev,re,r,rp)
	local chkf=Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and PLAYER_NONE or tp
	local mg=Duel.GetMatchingGroup(c35255456.filter1,tp,LOCATION_HAND+LOCATION_GRAVE+LOCATION_MZONE,0,nil,e)
	local sg=Duel.GetMatchingGroup(c35255456.filter2,tp,LOCATION_EXTRA,0,nil,e,tp,mg,chkf)
	if sg:GetCount()>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local tg=sg:Select(tp,1,1,nil)
		local tc=tg:GetFirst()
		local mat=Duel.SelectFusionMaterial(tp,tc,mg,nil,chkf)
		local cf=mat:Filter(c35255456.cffilter,nil)
		if cf:GetCount()>0 then
			Duel.ConfirmCards(1-tp,cf)
		end
		Duel.SendtoDeck(mat,nil,2,REASON_EFFECT)
		Duel.BreakEffect()
		Duel.SpecialSummon(tc,0,tp,tp,true,false,POS_FACEUP)
	end
end
