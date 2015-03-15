--ヘリオス・トリス・メギストス
function c17286057.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c17286057.hspcon)
	e1:SetOperation(c17286057.hspop)
	c:RegisterEffect(e1)
	--atk/def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_SET_ATTACK)
	e2:SetValue(c17286057.value)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_SET_DEFENCE)
	c:RegisterEffect(e3)
	--spsummon
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(17286057,0))
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(EVENT_PHASE+PHASE_END)
	e4:SetRange(LOCATION_GRAVE)
	e4:SetCountLimit(1)
	e4:SetCondition(c17286057.spcon)
	e4:SetTarget(c17286057.sptg)
	e4:SetOperation(c17286057.spop)
	c:RegisterEffect(e4)
	--chain attack
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_EXTRA_ATTACK)
	e5:SetValue(1)
	e5:SetCondition(c17286057.atcon)
	c:RegisterEffect(e5)
end
function c17286057.hspcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>-1
		and Duel.CheckReleaseGroup(c:GetControler(),Card.IsCode,1,nil,80887952)
end
function c17286057.hspop(e,tp,eg,ep,ev,re,r,rp,c)
	local g=Duel.SelectReleaseGroup(tp,Card.IsCode,1,1,nil,80887952)
	Duel.Release(g,REASON_COST)
end
function c17286057.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_MONSTER)
end
function c17286057.value(e,c)
	return Duel.GetMatchingGroupCount(c17286057.filter,c:GetControler(),LOCATION_REMOVED,LOCATION_REMOVED,nil)*300
end
function c17286057.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsReason(REASON_BATTLE) and e:GetHandler():GetTurnID()==Duel.GetTurnCount()
end
function c17286057.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c17286057.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummonStep(c,0,tp,tp,false,false,POS_FACEUP) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(500)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_UPDATE_DEFENCE)
		c:RegisterEffect(e2)
	end
	Duel.SpecialSummonComplete()
end
function c17286057.atcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)>0
end
