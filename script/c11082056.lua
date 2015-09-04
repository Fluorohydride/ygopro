--クリティウスの牙
function c11082056.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,11082056+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c11082056.target)
	e1:SetOperation(c11082056.activate)
	c:RegisterEffect(e1)
	--add code
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_ADD_CODE)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetValue(10000060)
	c:RegisterEffect(e2)
end
function c11082056.tgfilter(c,e,tp)
	return c:IsType(TYPE_TRAP) and Duel.IsExistingMatchingCard(c11082056.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp,c:GetCode())
end
function c11082056.spfilter(c,e,tp,code)
	return c:IsType(TYPE_FUSION) and c.material_trap and c:IsCanBeSpecialSummoned(e,0,tp,true,false) and code==c.material_trap
end
function c11082056.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c11082056.tgfilter,tp,LOCATION_HAND+LOCATION_SZONE,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c11082056.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g=Duel.SelectMatchingCard(tp,c11082056.tgfilter,tp,LOCATION_HAND+LOCATION_SZONE,0,1,1,nil,e,tp)
	local tc=g:GetFirst()
	if tc and not tc:IsImmuneToEffect(e) then
		if tc:IsFacedown() then Duel.ConfirmCards(1-tp,tc) end
		Duel.SendtoGrave(tc,REASON_EFFECT)
		if not tc:IsLocation(LOCATION_GRAVE) then return end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=Duel.SelectMatchingCard(tp,c11082056.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,tc:GetCode())
		local sc=sg:GetFirst()
		if sc then
			Duel.BreakEffect()
			Duel.SpecialSummon(sc,0,tp,tp,true,false,POS_FACEUP)
			sc:CompleteProcedure()
		end
	end
end
