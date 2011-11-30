--X－セイバー パロムロ
function c96099959.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(96099959,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c96099959.condition)
	e1:SetCost(c96099959.cost)
	e1:SetTarget(c96099959.target)
	e1:SetOperation(c96099959.operation)
	c:RegisterEffect(e1)
end
function c96099959.filter(c,ec,tp)
	return c~=ec and c:IsSetCard(0xd) and c:IsLocation(LOCATION_GRAVE) and c:GetPreviousControler()==tp
		and c:IsReason(REASON_BATTLE)
end
function c96099959.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c96099959.filter,1,nil,e:GetHandler(),tp)
end
function c96099959.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,500) end
	Duel.PayLPCost(tp,500)
end
function c96099959.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c96099959.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
