--竜脚獣ブラキオン
function c41753322.initial_effect(c)
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_SINGLE_RANGE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetRange(LOCATION_DECK)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--summon with 1 tribute
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(41753322,0))
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SUMMON_PROC)
	e2:SetCondition(c41753322.otcon)
	e2:SetOperation(c41753322.otop)
	e2:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e2)
	--turn set
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(41753322,1))
	e3:SetCategory(CATEGORY_POSITION)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetTarget(c41753322.postg)
	e3:SetOperation(c41753322.posop)
	c:RegisterEffect(e3)
	--position
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(41753322,1))
	e4:SetCategory(CATEGORY_POSITION)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	e4:SetTarget(c41753322.postg2)
	e4:SetOperation(c41753322.posop2)
	c:RegisterEffect(e4)
	--damage amp
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e5:SetRange(LOCATION_MZONE)
	e5:SetCode(EVENT_PRE_BATTLE_DAMAGE)
	e5:SetCondition(c41753322.dcon)
	e5:SetOperation(c41753322.dop)
	c:RegisterEffect(e5)
end
function c41753322.otfilter(c,tp)
	return c:IsRace(RACE_DINOSAUR) and (c:IsControler(tp) or c:IsFaceup())
end
function c41753322.otcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local mg=Duel.GetMatchingGroup(c41753322.otfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	return c:GetLevel()>6 and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.GetTributeCount(c,mg)>0
end
function c41753322.otop(e,tp,eg,ep,ev,re,r,rp,c)
	local mg=Duel.GetMatchingGroup(c41753322.otfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	local sg=Duel.SelectTribute(tp,c,1,1,mg)
	c:SetMaterial(sg)
	Duel.Release(sg,REASON_SUMMON+REASON_MATERIAL)
end
function c41753322.postg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsCanTurnSet() and c:GetFlagEffect(41753322)==0 end
	c:RegisterFlagEffect(41753322,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_END,0,1)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,c,1,0,0)
end
function c41753322.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		Duel.ChangePosition(c,POS_FACEDOWN_DEFENCE)
	end
end
function c41753322.postg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(Card.IsCanTurnSet,tp,LOCATION_MZONE,LOCATION_MZONE,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c41753322.posop2(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsCanTurnSet,tp,LOCATION_MZONE,LOCATION_MZONE,e:GetHandler())
	Duel.ChangePosition(g,POS_FACEDOWN_DEFENCE)
end
function c41753322.dcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return ep~=tp and Duel.GetAttackTarget()==c
end
function c41753322.dop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeBattleDamage(ep,ev*2)
end
