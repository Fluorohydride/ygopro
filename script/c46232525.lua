--ヘルモスの爪
function c46232525.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,46232525+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c46232525.target)
	e1:SetOperation(c46232525.activate)
	c:RegisterEffect(e1)
	--add code
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_ADD_CODE)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetValue(10000070)
	c:RegisterEffect(e2)
end
function c46232525.tgfilter(c,e,tp,chkf)
	if chkf~=PLAYER_NONE and c:IsLocation(LOCATION_HAND) then return false end
	return c:IsType(TYPE_MONSTER) and Duel.IsExistingMatchingCard(c46232525.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp,c:GetRace())
end
function c46232525.spfilter(c,e,tp,race)
	return c:IsType(TYPE_FUSION) and c.material_race and c:IsCanBeSpecialSummoned(e,0,tp,true,false) and race==c.material_race
end
function c46232525.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local chkf=Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and PLAYER_NONE or tp
		return Duel.IsExistingMatchingCard(c46232525.tgfilter,tp,LOCATION_HAND+LOCATION_MZONE,0,1,nil,e,tp,chkf) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c46232525.activate(e,tp,eg,ep,ev,re,r,rp)
	local chkf=Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and PLAYER_NONE or tp
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g=Duel.SelectMatchingCard(tp,c46232525.tgfilter,tp,LOCATION_HAND+LOCATION_MZONE,0,1,1,nil,e,tp,chkf)
	local tc=g:GetFirst()
	if tc and not tc:IsImmuneToEffect(e) then
		if tc:IsFacedown() then Duel.ConfirmCards(1-tp,tc) end
		local race=tc:GetRace()
		Duel.SendtoGrave(tc,REASON_EFFECT)
		if not tc:IsLocation(LOCATION_GRAVE) then return end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=Duel.SelectMatchingCard(tp,c46232525.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,race)
		local sc=sg:GetFirst()
		if sc then
			Duel.BreakEffect()
			Duel.SpecialSummon(sc,0,tp,tp,true,false,POS_FACEUP)
			sc:CompleteProcedure()
		end
	end
end
