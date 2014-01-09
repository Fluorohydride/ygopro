--ゴーストリックの猫娘
function c24101897.initial_effect(c)
	--summon limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetCondition(c24101897.sumcon)
	c:RegisterEffect(e1)
	--turn set
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(24101897,0))
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTarget(c24101897.postg)
	e2:SetOperation(c24101897.posop)
	c:RegisterEffect(e2)
	--change pos
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(24101897,1))
	e3:SetCategory(CATEGORY_POSITION)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCondition(c24101897.condition)
	e3:SetTarget(c24101897.target)
	e3:SetOperation(c24101897.operation)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EVENT_SUMMON_SUCCESS)
	c:RegisterEffect(e4)
end
function c24101897.sfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x8d)
end
function c24101897.sumcon(e)
	return not Duel.IsExistingMatchingCard(c24101897.sfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c24101897.postg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsCanTurnSet() and c:GetFlagEffect(24101897)==0 end
	c:RegisterFlagEffect(24101897,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_END,0,1)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,c,1,0,0)
end
function c24101897.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		Duel.ChangePosition(c,POS_FACEDOWN_DEFENCE)
	end
end
function c24101897.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c24101897.sfilter,tp,LOCATION_MZONE,0,1,e:GetHandler())
end
function c24101897.filter(c,e)
	return c:IsFaceup() and c:IsLevelAbove(4) and c:IsCanTurnSet() and (not e or c:IsRelateToEffect(e))
end
function c24101897.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c24101897.filter,1,nil) end
	Duel.SetTargetCard(eg)
	local g=eg:Filter(c24101897.filter,nil)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c24101897.operation(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.IsExistingMatchingCard(c24101897.sfilter,tp,LOCATION_MZONE,0,1,e:GetHandler()) then return end
	local g=eg:Filter(c24101897.filter,nil,e)
	Duel.ChangePosition(g,POS_FACEDOWN_DEFENCE)
end
