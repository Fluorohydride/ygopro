--氷結界の伝道師
function c50088247.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c50088247.spcon)
	e1:SetOperation(c50088247.spop)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(50088247,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCost(c50088247.spcost2)
	e2:SetTarget(c50088247.sptg2)
	e2:SetOperation(c50088247.spop2)
	c:RegisterEffect(e2)
end
function c50088247.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x2f)
end
function c50088247.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c50088247.cfilter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end
function c50088247.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c50088247.sumlimit)
	Duel.RegisterEffect(e1,tp)
end
function c50088247.sumlimit(e,c,sump,sumtype,sumpos,targetp,se)
	return c:GetLevel()>=5
end
function c50088247.spcost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleaseable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c50088247.filter(c,e,tp)
	return c:IsSetCard(0x2f) and c:GetCode()~=50088247 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c50088247.sptg2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c50088247.filter(chkc,e,tp) end
	if chk==0 then return Duel.IsExistingTarget(c50088247.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c50088247.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c50088247.spop2(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
