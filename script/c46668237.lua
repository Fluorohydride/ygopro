--森の番人グリーン·バブーン
function c46668237.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_HAND+LOCATION_GRAVE)
	e1:SetCode(EVENT_DESTROY)
	e1:SetOperation(c46668237.adjop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(46668237,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_HAND+LOCATION_GRAVE)
	e2:SetProperty(EFFECT_FLAG_CHAIN_UNIQUE)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetLabelObject(e1)
	e2:SetCondition(c46668237.condition)
	e2:SetCost(c46668237.cost)
	e2:SetTarget(c46668237.target)
	e2:SetOperation(c46668237.operation)
	c:RegisterEffect(e2)
end
function c46668237.filter(c)
	return c:IsFaceup() and c:IsLocation(LOCATION_MZONE) and c:IsRace(RACE_BEAST)
end
function c46668237.adjop(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c46668237.filter,nil)
	local tc=g:GetFirst()
	while tc do
		if tc:GetFlagEffect(46668237)==0 then
			tc:RegisterFlagEffect(46668237,RESET_EVENT+0x17a0000+RESET_PHASE+PHASE_END,0,1)
		end
		tc=g:GetNext()
	end
end
function c46668237.cfilter(c,tp)
	return c:IsType(TYPE_MONSTER) and c:IsRace(RACE_BEAST) and c:GetPreviousControler()==tp and c:GetFlagEffect(46668237)~=0
end
function c46668237.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c46668237.cfilter,1,nil,tp)
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
