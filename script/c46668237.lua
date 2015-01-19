--森の番人グリーン・バブーン
function c46668237.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(46668237,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_HAND+LOCATION_GRAVE)
	e1:SetProperty(EFFECT_FLAG_CHAIN_UNIQUE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c46668237.condition)
	e1:SetCost(c46668237.cost)
	e1:SetTarget(c46668237.target)
	e1:SetOperation(c46668237.operation)
	c:RegisterEffect(e1)
end
function c46668237.cfilter(c,tp)
	return c:IsType(TYPE_MONSTER) and c:IsRace(RACE_BEAST) and c:GetPreviousControler()==tp and c:IsPreviousPosition(POS_FACEUP)
		and c:IsPreviousLocation(LOCATION_MZONE) and bit.band(c:GetPreviousRaceOnField(),RACE_BEAST)~=0
end
function c46668237.condition(e,tp,eg,ep,ev,re,r,rp)
	return not eg:IsContains(e:GetHandler()) and eg:IsExists(c46668237.cfilter,1,nil,tp) and bit.band(r,REASON_DESTROY)~=0
end
function c46668237.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c46668237.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c46668237.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end
