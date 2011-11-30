--Ｔｈｅ　ｔｙｒａｎｔ　ＮＥＰＴＵＮＥ
function c88071625.initial_effect(c)
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(aux.FALSE)
	c:RegisterEffect(e1)
	--summon with 1 tribute
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(88071625,0))
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SUMMON_PROC)
	e2:SetCondition(c88071625.otcon)
	e2:SetOperation(c88071625.otop)
	e2:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e2)
	--atk
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_MATERIAL_CHECK)
	e3:SetValue(c88071625.valcheck)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e4:SetCode(EVENT_SUMMON_SUCCESS)
	e4:SetOperation(c88071625.atkop)
	c:RegisterEffect(e4)
	--copy
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(88071625,1))
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e5:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e5:SetCode(EVENT_SUMMON_SUCCESS)
	e5:SetCondition(c88071625.copycon)
	e5:SetTarget(c88071625.copytg)
	e5:SetOperation(c88071625.copyop)
	c:RegisterEffect(e5)
end
function c88071625.otcon(e,c)
	if c==nil then return true end
	return c:GetLevel()>6 and Duel.GetTributeCount(c)>0
end
function c88071625.otop(e,tp,eg,ep,ev,re,r,rp,c)
	local sg=Duel.SelectTribute(tp,c,1,1)
	c:SetMaterial(sg)
	Duel.Release(sg, REASON_SUMMON+REASON_MATERIAL)
end
function c88071625.valcheck(e,c)
	local g=c:GetMaterial()
	c88071625.atk=g:GetSum(Card.GetBaseAttack)
	c88071625.def=g:GetSum(Card.GetBaseDefence)
end
function c88071625.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetMaterialCount()==0 then return end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c88071625.atk)
	e1:SetReset(RESET_EVENT+0x1ff0000)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	e2:SetValue(c88071625.def)
	c:RegisterEffect(e2)
end
function c88071625.copycon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE
end
function c88071625.filter(c)
	return c:IsType(TYPE_EFFECT) and c:IsLocation(LOCATION_GRAVE)
end
function c88071625.copytg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return e:GetHandler():GetMaterial():IsContains(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=e:GetHandler():GetMaterial():FilterSelect(tp,c88071625.filter,1,1,nil)
	Duel.SetTargetCard(g)
end
function c88071625.copyop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and c:IsRelateToEffect(e) and c:IsFaceup() then
		local code=tc:GetOriginalCode()
		local reset_flag=RESET_EVENT+0x1fe0000
		c:CopyEffect(code, reset_flag, 1)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(reset_flag)
		e1:SetCode(EFFECT_CHANGE_CODE)
		e1:SetValue(code)
		c:RegisterEffect(e1)
	end
end
