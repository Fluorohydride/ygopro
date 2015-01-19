--サイバー・ダイナソー
function c39439590.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(39439590,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c39439590.spcon)
	e1:SetTarget(c39439590.sptg)
	e1:SetOperation(c39439590.spop)
	c:RegisterEffect(e1)
end
function c39439590.cfilter(c,tp)
	return c:GetSummonPlayer()==1-tp and c:IsPreviousLocation(LOCATION_HAND)
end
function c39439590.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c39439590.cfilter,1,nil,tp)
end
function c39439590.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c39439590.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end
