--溶岩魔神ラヴァ·ゴーレム
function c102380.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(102380,0))
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_SPSUM_PARAM)
	e1:SetRange(LOCATION_HAND)
	e1:SetTargetRange(POS_FACEUP,1)
	e1:SetCondition(c102380.spcon)
	e1:SetOperation(c102380.spop)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetDescription(aux.Stringid(102380,1))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_REPEAT)
	e2:SetCountLimit(1)
	e2:SetCondition(c102380.damcon)
	e2:SetTarget(c102380.damtg)
	e2:SetOperation(c102380.damop)
	c:RegisterEffect(e2)
	--cannot normal summon
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetOperation(c102380.op2)
	c:RegisterEffect(e3)
end
function c102380.spcon(e,c)
	if c==nil then return true end
	local p=c:GetControler()
	return not Duel.CheckNormalSummonActivity(p) and Duel.IsExistingMatchingCard(Card.IsReleaseable,p,0,LOCATION_MZONE,2,nil)
end
function c102380.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local g=Duel.SelectMatchingCard(tp,Card.IsReleaseable,tp,0,LOCATION_MZONE,2,2,nil)
	Duel.Release(g,REASON_COST)
end
function c102380.damcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c102380.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,0,0,tp,1000)
end
function c102380.damop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Damage(e:GetHandler():GetControler(),1000,REASON_EFFECT)
end
function c102380.op2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local sp=c:GetSummonPlayer()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetReset(RESET_PHASE+RESET_END)
	e1:SetTargetRange(1,0)
	Duel.RegisterEffect(e1,sp)
	local e2=e1:Clone(e1)
	e2:SetCode(EFFECT_CANNOT_MSET)
	Duel.RegisterEffect(e2,sp)
end
