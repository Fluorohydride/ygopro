--ジュラック・ヘレラ
function c16111820.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(16111820,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_HAND+LOCATION_GRAVE)
	e1:SetProperty(EFFECT_FLAG_CHAIN_UNIQUE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c16111820.condition)
	e1:SetTarget(c16111820.target)
	e1:SetOperation(c16111820.operation)
	c:RegisterEffect(e1)
end
function c16111820.filter(c,tp)
	return c:GetPreviousControler()==tp and c:IsPreviousPosition(POS_DEFENCE) and c:IsReason(REASON_BATTLE) and c:IsLocation(LOCATION_GRAVE) 
		and c:IsSetCard(0x22) and c:GetCode()~=16111820
end
function c16111820.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c16111820.filter,1,nil,tp)
end
function c16111820.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c16111820.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end
