--炎獄魔人ヘル·バーナー
function c23309606.initial_effect(c)
	--summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(23309606,0))
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_LIMIT_SUMMON_PROC)
	e1:SetCondition(c23309606.otcon)
	e1:SetOperation(c23309606.otop)
	e1:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e1)
	--atk
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetValue(c23309606.val)
	c:RegisterEffect(e2)
end
function c23309606.otcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local hg=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	hg:RemoveCard(c)
	local g=Duel.GetTributeGroup(c)
	return hg:GetCount()>0 and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and g:IsExists(Card.IsAttackAbove,1,nil,2000)
		and hg:FilterCount(Card.IsAbleToGraveAsCost,nil)==hg:GetCount()
end
function c23309606.otop(e,tp,eg,ep,ev,re,r,rp,c)
	local hg=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	hg:RemoveCard(c)
	Duel.SendtoGrave(hg,REASON_COST+REASON_DISCARD)
	local g=Duel.GetTributeGroup(c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local sg=g:FilterSelect(tp,Card.IsAttackAbove,1,1,nil,2000)
	c:SetMaterial(sg)
	Duel.Release(sg,REASON_SUMMON+REASON_MATERIAL)
end
function c23309606.val(e,c)
	local tp=c:GetControler()
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)*-500+500+Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)*200
end
