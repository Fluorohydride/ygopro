--幻獣ロックリザード
function c70969517.initial_effect(c)
	--summon with 1 tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(70969517,0))
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c70969517.otcon)
	e1:SetOperation(c70969517.otop)
	e1:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e1)
	--damage1
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(70969517,1))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCode(EVENT_BATTLE_DESTROYING)
	e2:SetTarget(c70969517.damtg1)
	e2:SetOperation(c70969517.damop)
	c:RegisterEffect(e2)
	--damage2
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(70969517,2))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetCondition(c70969517.damcon2)
	e2:SetTarget(c70969517.damtg2)
	e2:SetOperation(c70969517.damop)
	c:RegisterEffect(e2)
end
function c70969517.otfilter(c,tp)
	return c:IsSetCard(0x1b) and (c:IsControler(tp) or c:IsFaceup())
end
function c70969517.otcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local mg=Duel.GetMatchingGroup(c70969517.otfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	return c:GetLevel()>6 and Duel.GetTributeCount(c,mg)>0
end
function c70969517.otop(e,tp,eg,ep,ev,re,r,rp,c)
	local mg=Duel.GetMatchingGroup(c70969517.otfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	local sg=Duel.SelectTribute(tp,c,1,1,mg)
	c:SetMaterial(sg)
	Duel.Release(sg,REASON_SUMMON+REASON_MATERIAL)
end
function c70969517.damtg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(500)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,500)
end
function c70969517.damcon2(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,0x41)==0x41 and rp~=tp and e:GetHandler():GetPreviousControler()==tp
end
function c70969517.damtg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(2000)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,2000)
end
function c70969517.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
